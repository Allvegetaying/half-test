from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PROTOCOL_C = ROOT / "main" / "protocol" / "protocol.c"
PROTOCOL_H = ROOT / "main" / "protocol" / "protocol.h"
TEST_C = ROOT / "main" / "test.c"


def _crc16_kermit(payload: bytes) -> int:
    crc = 0
    for byte in payload:
        crc ^= byte
        for _ in range(8):
            crc = (crc >> 1) ^ 0x8408 if crc & 1 else crc >> 1
    return crc


def test_protocol_builds_the_uart0_semi_result_frame() -> None:
    success_payload = b"TPMS_S01,SEMI_RESULT,0,A1B2C3D4E5F6,101.3,24.5,0.98,3.25"
    fail_payload = b"TPMS_S01,SEMI_RESULT,1,A1B2C3D4E5F6,101.3,24.5,0.98,3.25"

    # 规范示例：成功 6389，失败 6EB0
    assert _crc16_kermit(success_payload) == 0x6389
    assert _crc16_kermit(fail_payload) == 0x6EB0
    assert "proto_build_semi_result" in PROTOCOL_H.read_text(encoding="utf-8")

    source = PROTOCOL_C.read_text(encoding="utf-8")
    assert '"TPMS_S01,SEMI_RESULT,%u,%s,%.1f,%.1f,%.2f,%.2f"' in source
    assert "proto_build_semi_test" not in source


def test_uart0_reports_parsed_semi_test_data_with_result() -> None:
    source = TEST_C.read_text(encoding="utf-8")

    assert "static proto_semi_t uart1_semi;" in source
    assert "uart1_semi = st;" in source
    # 上报用本地副本，BAT_V 由工装 ADC 实测值替换
    assert "proto_build_semi_result(frame, sizeof(frame), result, &semi)" in source
    assert "uart0_send_string(frame);" in source
    assert "static volatile int battery_adc_last_mv = -1;" in source
    assert "semi.bat_v = (float)adc_mv / 1000.0f;" in source


def test_new_detection_rounds_clear_cached_uart1_data() -> None:
    source = TEST_C.read_text(encoding="utf-8")

    for function in ("reset_compare_state", "sensor_wakeup_sequence"):
        start = source.index(f"static void {function}")
        body = source[start:source.index("\n}", start) + 2]
        assert "memset(&uart1_semi, 0, sizeof(uart1_semi));" in body


if __name__ == "__main__":
    test_protocol_builds_the_uart0_semi_result_frame()
    test_uart0_reports_parsed_semi_test_data_with_result()
    test_new_detection_rounds_clear_cached_uart1_data()
