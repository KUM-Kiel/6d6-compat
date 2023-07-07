# Change Log
All notable changes to this project will be documented in this file.

## [Unreleased]

## [1.6.8] - 2023-07-07
### Changed
- Updated leapsecond information according to Bulletin C 66 (2023-07-04).
### Fixed
- Fixed a memory bug when using an invalid placeholder in the output template.

## [1.6.7] - 2023-01-31
### Changed
- Updated leapsecond information according to Bulletin C 65 (2023-01-09).
### Fixed
- Corrected all leapseconds before the year 2000.

## [1.6.6] - 2022-08-01
### Changed
- Template option for julian day documented for `6d6mseed`.
- Corrected an error in the julian day calculation.
- Updated leapsecond information according to Bulletin C 64 (2022-07-05).

## [1.6.5] - 2022-01-31
### Changed
- Updated leapsecond information according to Bulletin C 63 (2022-01-05).

## [1.6.4] - 2021-08-02
### Added
- JSON progress for `6d6mseed` and `6d6read`.

### Changed
- Updated leapsecond information according to Bulletin C 62 (2021-07-05).

## [1.6.3] - 2021-01-19
### Added
- New tool `6d6strip`.

### Changed
- Updated leapsecond information according to Bulletin C 61 (2021-01-07).

## [1.6.2] - 2020-08-28
### Changed
- Changed the way file splits are calculated. This should stop sometimes missing a split.
- Using fewer timestamps to improve glitch resistance.

## [1.6.1] - 2020-07-20
### Changed
- Updated leapsecond information according to Bulletin C 60 (2020-07-07).

## [1.6.0] - 2020-04-29
### Added
- Added a `--resample` flag to `6d6mseed` which will resample the data to the nominal samplerate.

## [1.5.4] - 2020-01-29
### Changed
- Updated leapsecond information according to Bulletin C 59 (2020-01-07).

## [1.5.3] - 2019-07-05
### Changed
- Updated leapsecond information according to Bulletin C 58 (2019-07-04).

## [1.5.2] - 2019-06-12
### Added
- Added JSON output to `6d6info` via the `--json` flag.
- Added JSON progress output to `6d6copy` via the `--json-progress` flag.

## [1.5.1] - 2019-03-11
### Added
- `6d6read` now also supports `--start-time` and `--end-time`.

### Fixed
- Fixed an issue that prevented `6d6mseed` from emitting the last block of data when using the `--end-time` option.

## [1.5.0] - 2019-02-27
### Added
- `6d6mseed` now has the additional options `--no-cut`, `--start-time` and `--end-time`.

## [1.4.3] - 2019-01-22
### Added
- `6d6copy` can now add text to the comment header while copying.

### Changed
- Updated leapsecond information according to Bulletin C 57 (2019-01-07).

## [1.4.2] - 2018-08-10
### Changed
- `6d6info` and the logfile of `6d6mseed` now contain the channel names and gains.

## [1.4.1] - 2018-07-13
### Changed
- Updated leapsecond information according to Bulletin C 56 (2018-07-08).

## [1.4.0] - 2018-04-20
### Added
- `6d6mseed` now supports channel renaming with the `--channels=...` option.
- `6d6mseed` now has an undocumented and unsupported flag `--ignore-skew` to deactivate the skew correction.

## [1.3.2] - 2018-03-06
### Changed
- `6d6mseed` and `6d6read` can now read files with microsecond timestamps.

### Fixed
- The skew in `6d6read` had the wrong sign.

## [1.3.1] - 2018-01-10
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
