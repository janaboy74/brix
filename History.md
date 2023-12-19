# History<br>
#### 2023 Dec 19 08:21:04
- Ininializer list for corestorage classes added / fixed.
- Ininializer list tests added for corestorage classes.

#### 2023 Dec 18 19:19:41
- Addedd missing initialization list constructor for coreset and coremap and derivatives.

#### 2023 Dec 18 09:28:04
- The template class coremap and coreset has now optional modificators with different names.
- Test #2 updated to follow this.

#### 2023 Dec 17 19:43:20
- Helper class coresetmodificator implemented.
- Updated storage test #2.

#### 2023 Dec 15 12:16:52
- Fixed obvious crash problem in jsonparser.
- New error handling solution for parse in jsonparser.
- Replaced isEmpty with empty.

#### 2023 Dec 14 18:10:43
- New module jsonparser added.

#### 2023 Dec 14 18:10:09
- Extra semicolon removed from contructors in corestring.

#### 2023 Dec 14 17:31:41
- Added core namespace for core classes.
- Removed < using namespace std > for better compatibility.
- Removed .h extension from template classes.
- Added comment to looprecord function to describe the functionality of it.
- Complete rework of random number generator rng.
- Added hashing class based on the rng class and a test for it.
- Optional testing for correctness of rng class added.

#### 2023 Dec 1 09:01:54
- The missing const added to all components of corestorage to follow the particular conventions.
- Nice trick added to coremap that can help with rage based ( also the conventional method ) loop to modify the map in the loop ( + exmaples ).

#### 2023 Nov 30 22:52:10
- Added missing constructor for corevector in corestorage.

#### 2023 Nov 30 10:07:57
**corestorage:**
- corevars: bug fixes
- corevector: new init option.

**test2:**
- bugfix
- vector + init test

#### 2023 Nov 30 09:13:16
- Added missing rng-single test of rng.

#### 2023 Nov 29 07:11:47
- LICENSE Created.

#### 2023 Nov 25 10:05:25
- Random number generator added rng + get_rnd.

#### 2023 Nov 24 15:13:46
- Added countertest for looprecord-single.

#### 2023 Nov 24 10:34:47
- Type size_t added to corevars.

#### 2023 Nov 24 10:21:26
- Component corestring added.

#### 2023 Nov 24 10:17:52
- Single looprecord test extended.

#### 2023 Nov 24 10:15:39
- Project corestorage fixed and added new test.

#### 2023 Nov 24 10:13:02
- Added Qt and CMake filters.

#### 2023 Nov 24 09:55:09
- Qt stuff removed from CMakeLists.txt.

#### 2023 Nov 22 18:48:49
- Added corestorage to repo.

#### 2023 Nov 22 11:03:24
- Class looprecord updated.

#### 2023 Nov 22 10:55:13
- Using different names for single and class variant of looprecord.

#### 2023 Nov 22 10:43:52
- Added exception test to project looprecord.

#### 2023 Nov 22 10:21:16
- looprecord project added.

#### 2023 Nov 21 07:33:53
- Initial commit
