# UART0 SEMI_TEST Report Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace UART0's SEMI_RESULT report with a CRC-protected SEMI_TEST report containing parsed sensor data.

**Architecture:** Preserve UART1 validation and the UART1/RF readiness gate. Add a SEMI_TEST builder in the protocol module, retain the parsed UART1 `proto_semi_t` in `test.c`, and serialize it over UART0 when the existing compare lifecycle completes.

**Tech Stack:** ESP-IDF C, Python pytest source-level regression tests, the existing CRC16 implementation.

## Global Constraints

- UART0 emits `$TPMS_S01,SEMI_TEST,<CHIP_ID>,<PRESS>,<TEMP>,<ACC_Z>,<BAT_V>,<CRC16>#\r\n`.
- PRESS/TEMP use one decimal; ACC_Z/BAT_V use two decimals.
- `TPMS_S01,SEMI_TEST,A1B2C3D4E5F6,101.3,24.5,0.98,3.25` has CRC `6AA8`.
- UART0 waits for UART1 and RF readiness; RF stays an internal comparison input.
- Rejected UART1 frames do not replace cached data.

---

### Task 1: Create the Protocol-Level SEMI_TEST Builder

**Files:**
- Create: `tests/test_uart0_semi_test_report.py`
- Modify: `main/protocol/protocol.h:43-49`
- Modify: `main/protocol/protocol.c:137-160`

**Interfaces:**
- Produces `int proto_build_semi_test(char *out, size_t out_size, const proto_semi_t *semi)`.
- Returns the complete frame length on success and `-1` for null input or insufficient output space.

- [ ] **Step 1: Write the failing test**

Create `tests/test_uart0_semi_test_report.py`:

```python
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PROTOCOL_C = ROOT / "main" / "protocol" / "protocol.c"
PROTOCOL_H = ROOT / "main" / "protocol" / "protocol.h"


def _crc16_kermit(payload: bytes) -> int:
    crc = 0
    for byte in payload:
        crc ^= byte
        for _ in range(8):
            crc = (crc >> 1) ^ 0x8408 if crc & 1 else crc >> 1
    return crc


def test_protocol_builds_the_uart0_semi_test_frame() -> None:
    payload = b"TPMS_S01,SEMI_TEST,A1B2C3D4E5F6,101.3,24.5,0.98,3.25"
    assert _crc16_kermit(payload) == 0x6AA8
    assert "proto_build_semi_test" in PROTOCOL_H.read_text(encoding="utf-8")
    source = PROTOCOL_C.read_text(encoding="utf-8")
    assert '"TPMS_S01,SEMI_TEST,%s,%.1f,%.1f,%.2f,%.2f"' in source
    assert "proto_build_semi_result" not in source
```

- [ ] **Step 2: Confirm the test is red**

Run `pytest tests/test_uart0_semi_test_report.py::test_protocol_builds_the_uart0_semi_test_frame -v`.

Expected: FAIL because the new builder and format string do not exist, while the SEMI_RESULT builder does.

- [ ] **Step 3: Implement the minimal builder**

Replace the header declaration with:

```c
int proto_build_semi_test(char *out, size_t out_size, const proto_semi_t *semi);
```

Replace `proto_build_semi_result` with:

```c
int proto_build_semi_test(char *out, size_t out_size, const proto_semi_t *semi)
{
    char body[PROTO_FRAME_MAX + 1];
    if (!out || !semi || out_size == 0)
        return -1;
    int body_len = snprintf(body, sizeof(body),
                            "TPMS_S01,SEMI_TEST,%s,%.1f,%.1f,%.2f,%.2f",
                            semi->chip_id, semi->press, semi->temp,
                            semi->acc_z, semi->bat_v);
    if (body_len < 0 || body_len >= (int)sizeof(body))
        return -1;
    uint16_t crc = CRC16((const uint8_t *)body, body_len);
    int frame_len = snprintf(out, out_size, "$%s,%04X#\r\n", body, crc);
    return (frame_len < 0 || frame_len >= (int)out_size) ? -1 : frame_len;
}
```

- [ ] **Step 4: Confirm the test is green**

Run `pytest tests/test_uart0_semi_test_report.py::test_protocol_builds_the_uart0_semi_test_frame -v`.

Expected: PASS, including the independent `6AA8` CRC assertion.

- [ ] **Step 5: Commit Task 1**

Run `git add tests/test_uart0_semi_test_report.py main/protocol/protocol.h main/protocol/protocol.c`, then run `git commit -m "feat: build SEMI_TEST UART0 reports"`.

### Task 2: Cache UART1 Sensor Data and Report It Through UART0

**Files:**
- Modify: `tests/test_uart0_semi_test_report.py`
- Modify: `main/test.c:78-80,179-199,431-452`

**Interfaces:**
- Consumes `proto_build_semi_test` and the local `proto_semi_t st` created by `proto_parse_semi`.
- Produces one SEMI_TEST UART0 report after the existing readiness check succeeds.

- [ ] **Step 1: Extend the failing test**

Append:

```python
import re

TEST_C = ROOT / "main" / "test.c"


def _function_body(source: str, name: str) -> str:
    match = re.search(rf"static\s+\w+\s+{name}\s*\([^;{{}}]*\)\s*\{{", source)
    assert match, name
    brace, depth = source.index("{", match.start()), 0
    for index in range(brace, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}" and (depth := depth - 1) == 0:
            return source[brace:index + 1]
    raise AssertionError(name)


def test_uart0_reports_cached_uart1_data_after_rf_is_ready() -> None:
    source = TEST_C.read_text(encoding="utf-8")
    assert "static proto_semi_t uart1_semi;" in source
    assert "uart1_semi = st;" in _function_body(source, "on_uart1_frame")
    report = _function_body(source, "compare_and_report")
    assert "proto_build_semi_test(frame, sizeof(frame), &uart1_semi)" in report
    assert "uart0_send_string(frame);" in report
    assert "proto_build_semi_result" not in report
```

- [ ] **Step 2: Confirm the test is red**

Run `pytest tests/test_uart0_semi_test_report.py::test_uart0_reports_cached_uart1_data_after_rf_is_ready -v`.

Expected: FAIL because no cached struct exists and `compare_and_report` calls the old builder.

- [ ] **Step 3: Implement the smallest integration change**

Add state beside `uart1_chip_id`:

```c
static proto_semi_t uart1_semi;
```

Cache the parsed value after `proto_parse_semi` succeeds and before the ready bit:

```c
uart1_semi = st;
```

In `compare_and_report`, replace the old 96-byte frame and old builder call:

```c
char frame[PROTO_FRAME_MAX + 1];
if (proto_build_semi_test(frame, sizeof(frame), &uart1_semi) > 0)
    uart0_send_string(frame);
```

Use the `SEMI_TEST` log label, retain `result` only for the existing internal comparison, and clear the cache when a new detection round clears `uart1_chip_id`:

```c
memset(&uart1_semi, 0, sizeof(uart1_semi));
```

- [ ] **Step 4: Confirm the test is green**

Run `pytest tests/test_uart0_semi_test_report.py::test_uart0_reports_cached_uart1_data_after_rf_is_ready -v`.

Expected: PASS; the parsed UART1 struct is cached and passed to the UART0 builder.

- [ ] **Step 5: Run focused tests and commit Task 2**

Run `pytest tests/test_uart0_semi_test_report.py tests/test_adc_lifecycle.py -v`; expect PASS. Then run `git add main/test.c tests/test_uart0_semi_test_report.py` and `git commit -m "feat: report semi test data over UART0"`.

### Task 3: Verify the Complete Change

**Files:**
- Verify only: `main/protocol/protocol.c`, `main/test.c`, `tests/test_uart0_semi_test_report.py`, `tests/test_adc_lifecycle.py`

**Interfaces:** Completed protocol builder and UART0 integration.

- [ ] **Step 1: Run all host tests**

Run `pytest -v`.

Expected: PASS with no failures.

- [ ] **Step 2: Build the firmware**

Run `idf.py build`.

Expected: PASS; the renamed API and UART0 integration compile without errors.

- [ ] **Step 3: Reconfirm the contract**

Run `pytest tests/test_uart0_semi_test_report.py -v`.

Expected: PASS and the sample payload continues to calculate to `6AA8`.
