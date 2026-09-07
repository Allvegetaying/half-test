from pathlib import Path
import re


SOURCE = Path(__file__).resolve().parents[1] / "main" / "test.c"


def _function_body(source: str, name: str) -> str:
    start = source.index(f"static void {name}(")
    brace = source.index("{", start)
    depth = 0

    for index in range(brace, len(source)):
        char = source[index]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return source[brace:index + 1]

    raise AssertionError(f"Could not find function body for {name}")


def _clear_calls(body: str) -> list[str]:
    return re.findall(r"xEventGroupClearBits\s*\(\s*xEventFlags\s*,(.*?)\);", body, re.S)


def _set_calls(body: str) -> list[str]:
    return re.findall(r"xEventGroupSetBits\s*\(\s*xEventFlags\s*,(.*?)\);", body, re.S)


def test_adc_sampling_is_limited_to_active_detection_rounds() -> None:
    source = SOURCE.read_text(encoding="utf-8")

    assert "#define ADC_ENABLE_BIT" in source

    wakeup_body = _function_body(source, "sensor_wakeup_sequence")
    assert any("ADC_ENABLE_BIT" in call for call in _set_calls(wakeup_body))
    assert any("ADC_ENABLE_BIT" in call for call in _clear_calls(wakeup_body))

    for function in ("compare_and_report", "reset_compare_state", "worker_up_task"):
        body = _function_body(source, function)
        assert any("ADC_ENABLE_BIT" in call for call in _clear_calls(body)), function

    adc_body = _function_body(source, "adc_read_task")
    wait_pos = adc_body.find("xEventGroupWaitBits")
    read_pos = adc_body.find("battery_adc_read")
    assert wait_pos != -1
    assert wait_pos < read_pos


def test_product_detection_uses_gpio41_and_gpio42_as_lifecycle_inputs() -> None:
    source = SOURCE.read_text(encoding="utf-8")

    assert "#define CONTROL_GPIO_NUM1    GPIO_NUM_41" in source
    assert "#define CONTROL_GPIO_NUM2    GPIO_NUM_42" in source
    assert "#define DETECT_GPIO_NUM" not in source
    assert "button_reset_task" not in source
    assert 'xTaskCreate(pin_detect_task, "pin_detect"' in source

    read_state_body = _function_body(source, "product_detect_read_state")
    assert "level1 == 0 && level2 == 0" in read_state_body
    assert "level1 == 1 && level2 == 1" in read_state_body
    assert "PIN_INVALID" in read_state_body

    detect_body = _function_body(source, "pin_detect_task")
    assert "product_detect_read_state()" in detect_body
    assert "GPIO%d/GPIO%d 同时为低 -> 通知激活" in detect_body
    assert "GPIO%d/GPIO%d 同时为高 -> 通知待机" in detect_body
    assert "DETECT_GPIO_NUM" not in detect_body


if __name__ == "__main__":
    test_adc_sampling_is_limited_to_active_detection_rounds()
    test_product_detection_uses_gpio41_and_gpio42_as_lifecycle_inputs()
