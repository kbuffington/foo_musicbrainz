## 0.4.3.2
- Drop support for `Windows XP`/`Windows Vista`. `Windows 7` is the new minimum requirement.
- The main tagger dialog now prevents editing of items that are disabled in the `Preferences`.
- Revert all `ORIGINAL RELEASE DATE` `Preferences` that were added in `0.4.3` and `0.4.3.1`. It now gets written exactly as it did before - only when it differs from `DATE`.

## 0.4.2
- Ensure `Custom server` is respected everywhere, not just for the main lookup.
- Improve error message display when server is unavailable.
- Minor context menu fixes.

## 0.4.1
- Fix regression from `0.4.0` where releases weren't sorted by date.
- Show multiple formats in release list if present.
- Other cosmetic tweaks.

## 0.4.0
- The minimum requirement is now `foobar2000` `v1.4`.
- Rewrite component almost from the ground up to make use of the `JSON` webservice instead of `XML`. This makes it easier to maintain and add new features.
- Track titles are now generated from the release and not recording titles like they were previously.
- There have been major tweaks to the main dialog but I'm hoping everything should work as before. If not, please use the issues section to report any problems.
- Improve `Artist/Album` search results. It should be more noticeable with multi-disc lookups.
- Add support for writing `ASIN` and `ISRC` tags. Check the `Preferences` if you wish to turn this off.
- The tagger dialog is now resizeable. Thanks to @smoralis for the inspiration.

## 0.3.11
- Add Preference to always write `Album Artist` even for single artist albums.

## 0.3.10
- Update repository with latest `foobar2000` `SDK` and ensure all projects compile with `Visual Studio 2019`. No functional changes from previous version.

## 0.3.9
- Repsect `MusicBrainz` rate limits so searches that fetch several releases should complete.
- You can now paste full release into the `AlbumID` dialog and the URL will be stripped to leave just the `MBID`.

## 0.3.8
- Add `Preferences` for writing release country (`RELEASECOUNTRY`) and format (`MEDIA`) tags.

## 0.3.7
- Remove 99 track limit for `Artist/Album` and `AlbumID` lookups.

## 0.3.6
- Allow `TOC` lookups from files with a sample rate of 48000 Hz such as `Opus`.
- Enfore stricter checks on `TOC` submissions so only lossless files are supported.

## 0.3.5
- Default to `https`.
- Display a more helpful message when errors occur.

## 0.3.4
- Restore support for `Windows XP`.

## 0.3.3 
- Add support for setting a custom server in the `Preferences`.

## 0.3.2
- `foobar2000` `v1.3` is the new minimum requirement.
- Update repository with all pre-requisites meaning it should compile as-is with latest versions of `foobar2000` `SDK` and `Visual Studio`.
- Dialog tweaks.

## 0.3.1
- Last release by original developer.
