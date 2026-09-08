#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 半成品检测协议：串口 ASCII 帧
 *
 * 传感器 → 工装：$<型号>,SEMI_TEST,<CHIP_ID>,<PRESS>,<TEMP>,<ACC_Z>,<BAT_V>,<CRC16>#\r\n
 * 工装 → 上位机：$<型号>,SEMI_RESULT,<RESULT>,<CHIP_ID>,<PRESS>,<TEMP>,<ACC_Z>,<BAT_V>,<CRC16>#\r\n
 *               （RESULT=0 成功，1 失败；其余字段参照 SEMI_TEST 的传感器输出）
 *
 * CRC16 校验：复用 A7169/blt_gpio.h 里现成的 CRC16() 函数（原样使用，不改动），
 *             校验范围是 '$' 之后、最后一个 ',' 之间的 ASCII，结果高字节在前。 */

#define PROTO_FRAME_MAX      200     /* 单帧最长（含 $ 与 #） */
#define PROTO_MODEL_MAX       32
#define PROTO_CHIP_ID_MAX     12     /* CHIP_ID = vendor + sensor type + 4-byte sensor id */
#define PROTO_TOKEN_MAX       16     /* 一帧最多字段数 */

/* SEMI_TEST 解析结果 */
typedef struct {
    char  model[PROTO_MODEL_MAX];
    char  chip_id[PROTO_CHIP_ID_MAX + 1];   /* 12 hex chars plus '\0' */
    float press;   /* kPa */
    float temp;    /* ℃   */
    float acc_z;   /* g   */
    float bat_v;   /* V   */
} proto_semi_t;

/* 字节流分帧器状态 */
typedef struct {
    char     buf[PROTO_FRAME_MAX + 1];
    uint16_t len;
} proto_rx_t;

/* 完整帧回调：line 指向一帧 "$...#"，'\0' 结尾（不含尾部 \r\n） */
typedef void (*proto_frame_cb)(const char *line, void *ctx);

/* 校验一帧 CRC（复用 CRC16()：'$' 之后到最后 ',' 之间，高字节在前）。
 * 返回 0=通过，-1=失败；失败时 *calc_out 带回计算值，便于定位 DUT 实际算法。 */
int proto_crc_check(const char *line, uint16_t *calc_out);

/* 解析 SEMI_TEST 业务字段；返回 0=成功，-1=失败（含非 SEMI_TEST 帧） */
int proto_parse_semi(const char *line, proto_semi_t *out);

/* 构建 SEMI_RESULT 上报帧（工装 → 上位机）：
 * $<型号>,SEMI_RESULT,<RESULT>,<CHIP_ID>,<PRESS>,<TEMP>,<ACC_Z>,<BAT_V>,<CRC16>#\r\n
 * result 非 0 一律按 1（失败）发送；返回帧长，失败返回 -1。 */
int proto_build_semi_result(char *out, size_t out_size, uint8_t result,
                            const proto_semi_t *semi);
int proto_chip_id_equal(const char *left, const char *right);
int proto_build_rf_chip_id(char *out, size_t out_size,
                           uint8_t vendor_type, uint8_t sensor_type,
                           uint32_t sensor_id);

/* 喂入原始字节流，切出 "$...#" 完整帧并逐帧回调；返回本批切出的帧数 */
int proto_rx_feed(proto_rx_t *rx, const uint8_t *data, int len,
                  proto_frame_cb cb, void *ctx);

#endif /* PROTOCOL_H */
