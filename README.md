You can view the original readme [here](https://github.com/19379/foo_musicbrainz/blob/master/ORIGINAL README.md).

Check the [releases](https://github.com/19379/foo_musicbrainz/releases) page for the latest version.

My changes:

```
Allow TOC lookups from files with a sample rate of 48000 Hz such as Opus. Track sample
count must still match CD frame boundaries.

Ensure sample rate of all tracks are identical for TOC lookups.

Enforce stricter checks for TOC submissions so only lossless files are supported. This
affects MusicBrainz editors only.

Prevent background lookups if above TOC checks fail.

"Preferences" has a new "Custom server" option.

Add "Country" to "Date" column in main dialog.

Defaults to https.

Display a more helpful message when errors occur.

Fix bug where "Reset page" button in the "Preferences" didn't work on all fields.

Repository has been updated with all pre-requisites meaning it should compile as-is
with Visual Studio 2015.

Remove deprecated metadb lock/unlock methods and use get_info_ref() from the
current SDK (2015-08-03). Now requires foobar2000 v1.3 or above.
```
