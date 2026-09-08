# UART0 SEMI_TEST Report Design

## Scope

Replace the UART0 report frame emitted after a completed semi-finished test.
The new frame is:

```text
$TPMS_S01,SEMI_TEST,<CHIP_ID>,<PRESS>,<TEMP>,<ACC_Z>,<BAT_V>,<CRC16>#\r\n
```

For the sample values `A1B2C3D4E5F6,101.3,24.5,0.98,3.25`, the CRC is
`6AA8` using the project's existing CRC16 implementation.

## Behavior

- UART1 continues to validate and parse incoming `SEMI_TEST` sensor frames.
- The parsed sensor values are retained until the corresponding RF frame is
  available.
- UART0 sends the new `SEMI_TEST` report only after both UART1 and RF data are
  ready, preserving the existing comparison lifecycle.
- RF data remains an internal CHIP_ID comparison input and is not serialized
  into the UART0 report.
- The old UART0 `SEMI_RESULT` frame, which contained only CHIP_ID and RESULT,
  is removed.
- The report uses `%.1f` for PRESS and TEMP, and `%.2f` for ACC_Z and BAT_V.

## Implementation Boundaries

- `main/protocol`: replace the SEMI_RESULT frame builder with a SEMI_TEST frame
  builder that accepts parsed sensor data and appends CRC16.
- `main/test.c`: keep the latest parsed `proto_semi_t` value, pass it to the
  builder after the existing UART1/RF readiness gate, and update the log text.
- `tests`: add a regression test covering the sample payload and its expected
  complete UART0 frame including CRC.

## Error Handling

Malformed UART1 frames and invalid CRC values remain rejected before cached
sensor data is updated. A frame-builder failure continues to suppress UART0
transmission for that test round.
