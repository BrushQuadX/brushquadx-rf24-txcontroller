# Architecture

## Purpose

`BrushQuadX_RF24_TXController.ino` is a single-loop Arduino transmitter. Its only runtime responsibility is to sample pilot controls, normalize them into the radio payload format, and transmit that payload to the quadcopter receiver.

## Runtime flow

```text
setup()
	-> initialize RF24
	-> configure no auto-acknowledgement and 250KBPS
	-> open writing pipe
	-> initialize safe channel values
	-> configure serial and auxiliary input pins

loop()
	-> analogRead(A1, A0, A3, A2)
	-> constrain and calibrate each axis
	-> invert and map each axis to 0..255
	-> digitalRead(D2, D3)
	-> radio.write(&data, sizeof(MyData))
```

There is no explicit delay or scheduler. The loop runs as fast as the Arduino and RF24 library permit, and `radio.write` is the only transmission operation.

## Data model and protocol

The transmitted object is the six-byte `MyData` structure:

| Field | Type | Source | Meaning |
| --- | --- | --- | --- |
| `throttle` | `byte` | `A1` | Inverted joystick value |
| `yaw` | `byte` | `A0` | Inverted joystick value |
| `pitch` | `byte` | `A3` | Inverted joystick value |
| `roll` | `byte` | `A2` | Inverted joystick value |
| `AUX1` | `byte` | `D2` | Digital input state |
| `AUX2` | `byte` | `D3` | Digital input state |

The RF24 writing pipe is `0xE8E8F0F0E1`. The radio uses `RF24_250KBPS` and disables auto-acknowledgement. The receiver must agree with these radio settings and deserialize the fields in this exact order. The current transmitter has no acknowledgement, retry, packet sequence, timeout, or transmitter-side failsafe protocol.

## Inputs and normalization

`mapJoystickValues` first clamps each ADC reading to `0..1023`, then maps the lower half to `0..128` and the upper half to `128..255` around a per-axis center. The `reverse` argument is always `true` in the active V2 path, so the final value is `255 - mappedValue`.

Active V2 assignments and centers:

| Channel | Arduino input | Center |
| --- | --- | ---: |
| Throttle | `A1` | `516` |
| Yaw | `A0` | `523` |
| Pitch | `A3` | `527` |
| Roll | `A2` | `490` |

The two auxiliary pins are configured with `INPUT_PULLUP`; an unpressed/open switch normally reads high and a switch connected to ground reads low. Those raw digital states are transmitted as byte values without additional debouncing or semantic conversion.

## Initialization and safety behavior

`resetData()` sets throttle and both auxiliary channels to `0`, while yaw, pitch, and roll start at `127`. This is the initial in-memory state only; the first loop iteration replaces all six values with live input readings. No radio link-health check or loss-of-signal handling is implemented in this sketch, so receiver-side safety behavior is required.

## Hardware configuration

The active V2 RF24 configuration uses CE `D9` and CSN `D10`; SPI data and clock pins use the selected Arduino board's hardware SPI interface. Auxiliary inputs are `D2` and `D3`. A commented V1 configuration remains in the source with different joystick and RF24 pin mappings, but it is not part of the active runtime architecture.
