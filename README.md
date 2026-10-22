# RYB Demo 1

Source snapshot copied from the three PYNQ boards on 2026-10-22.

| Program folder | Board | Role |
| --- | --- | --- |
| `demo_1_no_crying_algorithm` | `10.43.0.1` | I2C master; crying module removed |
| `my_demo_1_heartbeat` | `10.43.0.2` | Heartbeat slave, address `0x10` |
| `my_demo_1_actuator` | `10.43.0.3` | Actuator slave, address `0x30` |

## Build and run

These programs require the existing `libpynq-5EWC0-2023-v0.2.6` installation. Copy each program folder into that board's `/home/student/libpynq-5EWC0-2023-v0.2.6/applications/` directory. The Makefiles use the installation's `applications/shared.mk` and `applications/end.mk`; the LCD font comes from its `fonts/` directory.

Inside the appropriate program folder:

```sh
make
./main
```

Start the heartbeat and actuator programs before starting the algorithm.

## Current behavior

- All three programs route IIC0 to `IO_AR_SDA` and `IO_AR_SCL`.
- The master reads heartbeat register `0x00` and writes actuator amplitude register `0x00` and frequency register `0x01`, using four-byte values.
- Frequency values are stored in hundredths of a hertz.
- The master has a 50 ms loop delay. The slave loops have no sleeps or monotonic timers; their LCDs update when values change.
- `algorithm_background.h` is preserved from the algorithm folder, but the current `main.c` does not include it.

The program sources are copied unchanged from the boards. Generated executables, object/dependency files, and old `.orig` backups are omitted. This upload is a source backup, not a confirmation that the full hardware communication test passes.
