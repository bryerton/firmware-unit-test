# Firmware Unit Tests

This project performs unit tests on the firmware drivers for the MVM

## Installation

Clone the repository, then from inside the repository type:

```
git submodule init
git submodule update --recursive
```

## Running

```
cd build
cmake ..
make all
make test
../run_gcovr.sh
```

## Viewing coverage report

The coverage reports are placed in the `report` directory, the top level can be found at `report/coverage.html`
