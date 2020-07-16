## Test Environment
- local windows, R 4.0.0
- rhub debian, R-Release
- win-builder, R-Devel

## R CMD Check Results
0 errors | 0 warnings | 1 notes

* checking CRAN incoming feasibility ... NOTE
Maintainer: 'Eric Anderson <anderic1@gmx.com>'


New submission
Possibly mis-spelled words in DESCRIPTION:
  Goldfarb (8:139)
  Idnani (8:156)
  
  
## Resubmission 2020-07-16
* Please do not start your description field in the DESCRIPTION file with
phrases like 'This is a R package', 'This package', the package name,
the package title or similar.  The Description field of the DESCRIPTION file is intended to be a (one
paragraph) description of what the package does and why it may be
useful. Please elaborate.
  - Hopefully changed the description to something that complies, otherwise please advise.
* Changed license to GPL >= 3 which as far as I can tell is compatible with the Apache v2 licensing of qpmad

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
