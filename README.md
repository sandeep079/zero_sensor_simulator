

## What is This?

**S24 Zero Sensor Simulator** is a lightweight, drop-in replacement for physical sensors during early firmware development and continuous integration (CI) testing. It provides mock implementations of five critical sensors—IMU, GPS, LiDAR, Camera, and Battery—that boot instantly and return deterministic baseline data.

### The Problem It Solves
Embedded software teams often waste valuable engineering cycles waiting for physical hardware, stable bench setups, or heavy-weight physics simulators just to test basic autopilot logic, telemetry pipelines, or mission software. S24 unblocks your software pipeline immediately by providing standard sensor interfaces **in minutes**.

### What It Doesn't Do
This is **not** a high-fidelity physics simulator. It does not model complex real-world environmental dynamics, kinematics, or intricate sensor noise. This design is highly intentional: **we optimize for boot speed, determinism, and execution reliability, not physical accuracy.**

---

## Key Features

*   **5 Sensor Stubs** — Instant mock data for IMU, GPS, LiDAR, Camera, and Battery.
*   **S09 HAL Contract** — Form-fits your existing Hardware Abstraction Layer (HAL) specifications exactly.
*   **Master Power Toggle** — Initialize, boot, or shut down all simulated sensors with a single unified call.
*   **Zero Memory Footprint** — Designed for embedded constraints with no dynamic memory leaks (fully validated via Valgrind).
*   **CI-Optimized** — Seamlessly integrates with continuous integration pipelines; includes pre-configured GitHub Actions.
*   **Single-Command Workflow** — Build, run tests, and perform static analysis using a single unified `./run.sh` script.

---

## Architecture Overview

```text
┌─────────────────────────────────────────────────────────────┐
│                    S24 Master Toggle                        │
│              s24_master_boot() / shutdown()                 │
└─────────────────────────┬───────────────────────────────────┘
                          │
      ┌───────────────────┼───────────────────┬─────────────────┐
      │                   │                   │                 │
      ▼                   ▼                   ▼                 ▼
┌──────────┐        ┌──────────┐        ┌──────────┐     ┌────────────┐
│   IMU    │        │   GPS    │        │  LiDAR   │     │  Battery   │
│ ICM-42688│        │ NEO-M9N  │        │ VLP-16   │     │  MAX17048  │
├──────────┤        ├──────────┤        ├──────────┤     ├────────────┤
│ 9.81m/s² │        │ SF, 50m  │        │  5.0m    │     │  12.4V, 98%│
│ 25°C     │        │ 3D Fix   │        │ 16-26 pts│     │  28°C      │
└──────────┘        └──────────┘        └──────────┘     └────────────┘
      │                   │                   │                 │
      └───────────────────┼───────────────────┴─────────────────┘
                          │
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                      S09 HAL Contract                       │
│             GPIO / SPI / I2C / Timer / Logging              │
└─────────────────────────────────────────────────────────────┘
```

---

## Getting Started

### Prerequisites

*   **Compiler**: GCC (or any C99-compatible compiler)
*   **Build Tools**: `make`, `build-essential`
*   **Dynamic Analysis**: `valgrind` (optional, for memory leak verification)

### Installation

```bash
# Clone the repository
git clone https://github.com/sandeep079/zero_sensor_simulator.git
cd zero_sensor_simulator

# Build, test, and run memory leaks validation instantly
./run.sh all
```

Upon successful execution, you will see confirmation in your terminal:

```text
 ALL TESTS PASSED - S24 Zero Sensor Simulator Ready
 No memory leaks detected!
```

---

## Usage

### Quick Commands Reference

| Command | Description |
| :--- | :--- |
| `./run.sh all` | Executes full verification pipeline (build + test + valgrind checks) |
| `./run.sh test` | Runs the test suite |
| `./run.sh build` | Compiles the simulator binaries |
| `./run.sh clean` | Deletes build artifacts |
| `./run.sh valgrind` | Runs memory leak analysis against simulated devices |
| `./run.sh ci` | Simulates the GitHub Actions CI pipeline execution locally |
| `./run.sh help` | Displays all available commands |

### Code Integration Example

```c
#include "s24_contract.h"
#include <stdio.h>

int main(void) {
    // 1. Boot all simulated sensors
    s24_master_boot();

    // 2. Read IMU telemetry
    s24_imu_data_t imu;
    if (s24_imu_read(&imu) == S24_OK) {
        printf("Accel: Z=%.2f m/s², Temp=%.1f°C\n", imu.z, imu.temperature);
    }

    // 3. Read GPS telemetry
    s24_gps_data_t gps;
    if (s24_gps_read(&gps) == S24_OK) {
        printf("Position: %.6f°N, %.6f°W @ %.1fm\n", 
               gps.latitude, gps.longitude, gps.altitude);
    }

    // 4. Graceful shutdown
    s24_master_shutdown();
    return 0;
}
```

---

## Sensor Register & Spec Reference

### IMU (`ICM-42688-P`)

| Register/Field | Default Simulated Value | Behavioral Description |
| :--- | :--- | :--- |
| **Acceleration** | `0.0, 0.0, 9.81` m/s² | Constant gravity on the Z-axis |
| **Temperature** | `25.0 ± 0.5` °C | Introduces slight random fluctuation over time |
| **WHO_AM_I** | `0x47` | Constant hardware register validation ID |

### GPS (`u-blox NEO-M9N`)

| Register/Field | Default Simulated Value | Behavioral Description |
| :--- | :--- | :--- |
| **Position** | `37.7749`° N, `122.4194`° W | Statically pinned to San Francisco, CA |
| **Altitude** | `50.0` m | Fixed baseline altitude |
| **Fix Quality** | `3` | Mimics a stable 3D GPS fix |
| **Speed** | `0.0` m/s | Represents a stationary receiver |

### LiDAR (`Velodyne VLP-16`)

| Register/Field | Default Simulated Value | Behavioral Description |
| :--- | :--- | :--- |
| **Distance** | `5.0 ± 0.5` m | Slowly oscillating distance tracking |
| **Intensity** | `80` to `120` | Simulated intensity fluctuation |
| **Point Count** | `16` to `26` | Points processed per frame cycle |

### Camera (`OV2640`)

| Register/Field | Default Simulated Value | Behavioral Description |
| :--- | :--- | :--- |
| **Resolution** | `800 × 600` | Static frame dimension properties |
| **Data Size** | `960,000` bytes | Calculated size for raw `RGB565` output |
| **Frame Rate** | Simulated time ticks | Stubs out time delays without loading binary image files |

### Battery (`MAX17048`)

| Register/Field | Default Simulated Value | Behavioral Description |
| :--- | :--- | :--- |
| **Voltage** | `12.4`V to `12.6`V | Slight discharge noise simulation |
| **Charge Status**| `98%` to `100%` | High-capacity baseline |
| **Current Draw** | `0.5`A to `0.6`A | Standard idle system current draw |

---

## Development

### Project Structure

```text
zero_sensor_simulator/
├── include/
│   ├── s24_contract.h      # HAL contract & sensor definitions
│   └── s24_logger.h        # Internal logging utilities
├── src/
│   ├── s24_master.c        # Central master toggle controller
│   ├── s24_logger.c        # Unified logging implementation
│   ├── sensor_imu.c        # IMU stub module
│   ├── sensor_gps.c        # GPS stub module
│   ├── sensor_lidar.c      # LiDAR stub module
│   ├── sensor_camera.c     # Camera stub module
│   └── sensor_battery.c    # Battery stub module
├── test/
│   └── test_harness.c      # Unit and system test suite
├── .github/workflows/
│   └── ci.yml              # GitHub Actions Workflow configuration
├── Makefile                # Multi-target build script
├── run.sh                  # One-command developer wrapper
└── README.md               # Documentation (This file)
```

### Adding a New Simulated Sensor

To scale the simulator with your hardware platform additions:

1.  **Duplicate an Existing Stub**: Copy an existing sensor file (e.g., `src/sensor_imu.c`).
2.  **Define Structures**: Add your data structures and error codes into `include/s24_contract.h`.
3.  **Implement API**: Expose standard initializer (`_init`), reader (`_read`), and teardown (`_deinit`) functions.
4.  **Register with Master Toggle**: Add your new sensor calls to the `s24_master_boot()` and `s24_master_shutdown()` sequences inside `src/s24_master.c`.
5.  **Write Tests**: Add a corresponding validation block inside `test/test_harness.c`.

### Manual Compilation

For granular development cycles, utilize standard GNU Make commands:

```bash
make clean    # Wipe previous builds
make          # Build simulator binary targets
make test     # Build and run the verification suites
make valgrind # Audit binaries for memory leaks
```

---

## Continuous Integration (CI)

The included `.github/workflows/ci.yml` pipeline automatically triggers on all branch push and pull requests to ensure stability:

*   **Compiler Compliance** — Audits the code using strict compilation standards (`-Wall -Wextra -Werror`).
*   **Unit Tests** — Asserts that mock sensor values remain within contract tolerances.
*   **Valgrind Check** — Asserts absolute-zero dynamic memory footprint leaks during boot/shutdown loops.
*   **Artifact Archival** — Packages and archives execution telemetry outputs for audit logs.

README.md
Displaying README.md.