You can view the original readme [here](ORIGINAL%20README.md).

My changes:

```
We now obey the rate limit rules with a delay inbetween each request
so any lookup with more than several releases should complete.

You can now paste full release URLs like

https://musicbrainz.org/release/76a601d9-a1a6-4112-aad5-d63bc85db9b6

in to the MBID query dialog and it will automatically strip the URL so
you're left with just the MBID like "76a601d9-a1a6-4112-aad5-d63bc85db9b6".
This is useful if a given artist/album has many releases and you find it
easier to find the correct one on the musicbrainz.org website. This will
only work with "release" URLs and not "release-group" and the track count
must always match the selection.

Using the artist/album lookup dialog will populate the album field
with the "TITLE" if there is no album tag present and the track
count is one. There are some releases with just one track on
Musicbrainz so this should make them easier to find.

Add preferences to write release country (RELEASECOUNTRY) and release
format (MEDIA). They're both enabled by default so check
File>Preferences>Tools>Tagging>MusicBrainz Tagger if you wish to
turn them off.

Rework tagger dialog layout so release country and format are shown.

Remove 99 track limit for artist/album and MBID lookups.

Allow TOC lookups from files with a sample rate of 48000 Hz such as
Opus. Track sample count must still match CD frame boundaries.

Ensure sample rate of all tracks are identical for TOC lookups.

Enforce stricter checks for TOC submissions so only lossless files
are supported. This affects MusicBrainz editors only.

Prevent background lookups if above TOC checks fail.

"Preferences" has a new "Custom server" option.

Defaults to https.

Display a more helpful message when errors occur.

Fix bug where "Reset page" button in the "Preferences" didn't
work on all fields.

Repository has been updated with all pre-requisites meaning
it should compile as-is with Visual Studio 2015.

Remove deprecated metadb lock/unlock methods and use get_info_ref()
from the current SDK (2015-08-03). Now requires foobar2000 v1.3 or above.
```
