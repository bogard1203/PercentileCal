## Description

This is a tool to compute various percentiles from given text files.
This tool also incorporate [streaming-percentiles](https://github.com/sengelha/streaming-percentiles), which is intended for extremely high-throughput data systems.

## BUILD:

Use 'cmake' and 'make' to build the executable.
```bash
mkdir build
cd build
cmake .. && make
```

To run unit-test after  build
```bash
ctest
```

If you build this on Windows, you may need [dirent](https://github.com/tronkko/dirent).

## USAGE:
The executable takes a config and a path as its inputs.
```bash
executable test/config_1.txt test/unit_1
```

The config file specifies a regex to match files in the given path and another regex for matching the line format in files. Please see examples in test folder.

To utilize streaming-percentiles, where epsilon is a float point value between 0 and 1.
```bash
executable -e epsilon test/config_1.txt test/unit_1
```

Config file contains pairs of key-value sets with keywords: 

FILE_REGEX \# Regular expression for filtering files

LINE_REGEX \# Regular expression parsing line format

GROUPID \# Specify the target token of the parsed result

PERCENTILE \# List intended percentiles to output

Please refer to config files under test folder.

## License

This project is licensed under the MIT License. [LICENSE](LICENSE)



