#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "protocol.h"
#include "blt_gpio.h"   /* 现成 CRC16() 校验函数，原样使用（含于 INCLUDE_DIRS "A7169"） */

/* ---------------------------------------------------------------------
 * 帧 CRC 校验：复用 CRC16()（blt_gpio.h），对 '$' 之后、最后一个 ','
 * 之间的 ASCII 计算，与帧尾 4 位 hex（高字节在前）比对。
 * ------------------------------------------------------------------- */
int proto_crc_check(const char *line, uint16_t *calc_out)
{
    int len = (int)strlen(line);
    int hash = len - 1;
    while (hash >= 0 && line[hash] != '#') hash--;
    if (hash < 0) return -1;

    int last = -1;
    for (int i = 0; i < hash; i++)
        if (line[i] == ',') last = i;
    if (last < 0 || hash - last - 1 != 4)   /* CRC 字段必须正好 4 位 hex */
        return -1;

    uint16_t expect = 0;
    for (int i = 0; i < 4; i++) 
    {
        char c = line[last + 1 + i];
        expect <<= 4;
        if      (c >= '0' && c <= '9') expect |= (c - '0');
        else if (c >= 'A' && c <= 'F') expect |= (c - 'A' + 10);
        else if (c >= 'a' && c <= 'f') expect |= (c - 'a' + 10);
        else return -1;
    }

    /* 重算范围：'$'(下标0) 之后 到 最后一个 ','(不含) 之间 */
    uint16_t calc = CRC16((const uint8_t *)line + 1, last - 1);
    if (calc_out) *calc_out = calc;
    return (calc == expect) ? 0 : -1;
}

/* ---------------------------------------------------------------------
 * 按逗号切令牌（含 model/cmd/crc），共享静态缓冲
 * ------------------------------------------------------------------- */
static char toks[PROTO_TOKEN_MAX][PROTO_FRAME_MAX + 1];

static int split_tokens(const char *line)
{
    int n = 0;
    const char *p = line;

    if (!line || line[0] != '$')
        return 0;
    p++;                                /* 跳过 '$' */

    while (*p && *p != '#' && n < PROTO_TOKEN_MAX) {
        int j = 0;
        while (*p && *p != ',' && *p != '#' && j < PROTO_FRAME_MAX)
            toks[n][j++] = *p++;
        toks[n][j] = '\0';
        n++;
        if (*p == ',') p++;
    }
    return n;
}

/* CHIP_ID 固定 4 字节 = 8 位 hex：长度必须恰好 8 且全为 hex 字符 */
static int chip_id_is_valid(const char *s)
{
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        char c = s[i];
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')))
            return 0;
    }
    return (i == PROTO_CHIP_ID_MAX);
}

int proto_parse_semi(const char *line, proto_semi_t *out)
{
    int n = split_tokens(line);

    if (!out || n < 7 || strcmp(toks[1], "SEMI_TEST") != 0 || !chip_id_is_valid(toks[2]))
        return -1;

    /* 字段索引：0=model 1=cmd 2=chip 3=press 4=temp 5=acc_z 6=bat_v */
    strncpy(out->model, toks[0], sizeof(out->model) - 1);
    out->model[sizeof(out->model) - 1] = '\0';
    strncpy(out->chip_id, toks[2], PROTO_CHIP_ID_MAX);
    out->chip_id[PROTO_CHIP_ID_MAX] = '\0';
    out->press = strtof(toks[3], NULL);
    out->temp  = strtof(toks[4], NULL);
    out->acc_z = strtof(toks[5], NULL);
    out->bat_v = strtof(toks[6], NULL);
    return 0;
}

int proto_build_semi_result(char *out, size_t out_size, const char *chip_id, uint8_t result)
{
    char body[PROTO_FRAME_MAX + 1];
    char safe_chip_id[PROTO_CHIP_ID_MAX + 1];
    int body_len;
    int frame_len;

    if (!out || !chip_id || out_size == 0)
        return -1;

    strncpy(safe_chip_id, chip_id, sizeof(safe_chip_id) - 1);
    safe_chip_id[sizeof(safe_chip_id) - 1] = '\0';

    body_len = snprintf(body, sizeof(body), "TPMS_S01,SEMI_RESULT,%s,%u",
                        safe_chip_id, result ? 1 : 0);
    if (body_len < 0 || body_len >= (int)sizeof(body))
        return -1;

    uint16_t crc = CRC16((const uint8_t *)body, body_len);
    frame_len = snprintf(out, out_size, "$%s,%04X#\r\n", body, crc);
    if (frame_len < 0 || frame_len >= (int)out_size)
        return -1;

    return frame_len;
}

/* ---------------------------------------------------------------------
 * 字节流分帧器：按 "$...#" 切帧，跨包/帧间垃圾自动处理
 * ------------------------------------------------------------------- */
int proto_rx_feed(proto_rx_t *rx, const uint8_t *data, int len,
                  proto_frame_cb cb, void *ctx)
{
    int frames = 0;

    if (!rx || !cb)
        return 0;

    /* 续接新字节 */
    for (int i = 0; i < len; i++) {
        if (rx->len >= PROTO_FRAME_MAX)
            rx->len = 0;                 /* 缓冲满仍未封帧：重同步 */
        rx->buf[rx->len++] = (char)data[i];
        rx->buf[rx->len] = '\0';
    }

    while (1) {
        int blen = rx->len;
        const char *b = rx->buf;

        /* 找 '$' 起点 */
        int s = -1;
        for (int i = 0; i < blen; i++) {
            if (b[i] == '$') { s = i; break; }
        }
        if (s < 0)
            break;

        /* 找对应的 '#' */
        int e = -1;
        for (int i = s + 1; i < blen; i++) {
            if (b[i] == '#') { e = i; break; }
        }
        if (e < 0) {
            /* 未封帧；若帧过长则丢弃到 '$' 之后重等 */
            if (blen - s >= PROTO_FRAME_MAX) {
                rx->len = 0;
                rx->buf[0] = '\0';
            }
            break;
        }

        /* 交付完整帧（$...#，不含尾部 \r\n） */
        char out[PROTO_FRAME_MAX + 1];
        int flen = e - s + 1;
        memcpy(out, &b[s], flen);
        out[flen] = '\0';
        cb(out, ctx);
        frames++;

        /* 消费本帧及可选的后缀 \r\n */
        int next = e + 1;
        if (next < blen && b[next] == '\r') next++;
        if (next < blen && b[next] == '\n') next++;
        memmove(rx->buf, &rx->buf[next], blen - next);
        rx->len = (uint16_t)(blen - next);
        rx->buf[rx->len] = '\0';
    }

    return frames;
}
