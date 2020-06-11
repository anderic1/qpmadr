## Test Environment
- local windows, R 4.0.0
- rhub debian, R-Release
- win-builder, R-Devel

## R CMD Check Results
0 errors | 0 warnings | 1 notes

Possibly mis-spelled words in DESCRIPTION:
    Goldfarb (11:20)
    Idnani (11:37)
    Sherikov (10:44)


## Resubmission 2020-06-11
* Please do not start your description field in the ESCRIPTION file with phrases like 'This is a R package', 'This package', the package name, the package title or similar.
  - Clarified description field in DESCRIPTION file


## Resubmission 2020-06-04
* please omit the redundant "R" in your Title.
  - removed R in title
* Please do not quote names of persons in your Description text. Please only single quote software names.
  - Removed quotes from 'Alexander Sherikov', 'D. Goldfarb', and 'A. Idnani'. This brings back one NOTE in the CRAN checks :
  Possibly mis-spelled words in DESCRIPTION:
    Goldfarb (11:20)
    Idnani (11:37)
    Sherikov (10:44)
* Alexander Sherikov is a copyright holder. Please add all authors 
and copyright holders in the Authors@R field with the appropriate roles.
  - changed to Authors@R field and added a cph+ctb role to Alexander Sherikov


## Resubmission 2020-05-28
- added \\value to qpmadParameters.Rd
- added example to function qpmadParameters
- amended \\value of solveqp.Rd
- added single quotes to names in the description file
- added doi reference of the method used to the Description file
