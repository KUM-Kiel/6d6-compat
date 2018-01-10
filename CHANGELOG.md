# Change Log
All notable changes to this project will be documented in this file.

## [Unreleased]
### Changed
- Updated leapsecond information according to Bulletin C 55 (2018-01-09).
- `6d6mseed` now mentions the creation of a logfile or an auxfile in the log.

### Fixed
- Fixed a segfault in `6d6mseed` when output pattern contains no dirname.
- Fixed a rare circumstance where sometimes an mseed file would not be split correctly.

## [1.3.0] - 2017-07-31
### Added
- `6d6info`, `6d6copy`, `6d6mseed` and `6d6read` now support multiple languages.
- `6d6mseed` now has an option `--auxfile` to create a CSV file with auxiliary engineering data like temperature and battery voltage.

## [1.2.6] - 2017-07-26
### Added
- Added a `--logfile` option to `6d6mseed`. The logfile contains information about events during processing.

## [1.2.5] - 2017-07-25
### Changed
- Better information display.
- Displayed information is now consistent for `6d6mseed` and `6d6info`.

## [1.2.4] - 2017-07-10
### Added
- macOS binaries are now available.

### Changed
- Updated leapsecond information.

## [1.2.3] - 2017-07-04
### Changed
- `6d6mseed` now has a `--cut` option to create files of different length than a day.

## [1.2.2] - 2017-04-24
### Changed
- `6d6mseed` now prints much more information.
- `6d6mseed` shows a warning when the leapsecond information is outdated.

## [1.2.1] - 2017-03-27
### Changed
- Skew is now saved to s2x files in microseconds.

## [1.2.0] - 2017-02-17
### Added
- New program `6d6info` to show information about a 6D6 file or an SD card.
- New program `6d6mseed` for conversion to MiniSEED.
- Packaging system. `rake package` will build the project and create a distributable file.

### Changed
- Changed the build flags to produce statically linked 32-bit binaries. This should hopefully give the best compatibility.

## [1.1.0] - 2016-10-21
### Changed
- Cleanup of the prototype.
- The project is now using git.
