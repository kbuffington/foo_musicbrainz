```
0.4.1
FIX: Fix regression from 0.4.0 where releases weren't sorted by date.
CHG: Show multiple formats in release list if present.
CHG: Other cosmetic tweaks.

0.4.0
CHG: The minimum requirement is now foobar2000 v1.4.
CHG: Rewrite component almost from the ground up to make use of the
     JSON webservice instead of XML. This makes it easier to maintain
     and add new features.
CHG: Track titles are now generated from the release and not recording
     titles like they were previously.
CHG: There have been major tweaks to the main dialog but I'm hoping
     everything should work as before. If not, please use the issues
     section to report any problems.
FIX: (Beta2) Fixed a bug where discnumber/totaldiscs wasn't always
     written as expected for multiple disc releases. It has never been
     written for single disc releases so that isn't a bug!
FIX: (Beta3) Restore support for multi-disc lookups. You can preview
     all tracks at once. Note that editing the disc subtitle in the
     track list only works when you select track #1 from each disc.
     Updates are applied to all tracks when tagging.
FIX: (Beta4) "Album artist" tags should be constructed from all artist
     credits, not just the first one.
FIX: (Beta5) Improve Artist/Album search results. It should be more
     noticeable with multi-disc lookups.
NEW: (Beta6) Add support for writing ASIN and ISRC tags. Check the
     Preferences if you wish to turn this off.
CHG: (Beta6) Hide the "Disc Subtitle" column for single disc releases.
FIX: (Beta7) Properly support multi-value ISRC tags.
FIX: (Beta7) Fix a tagger dialog bug which caused a component crash!
NEW: (Beta8) The tagger dialog is now resizeable. Thanks to @smoralis
     for the inspiration.

0.3.11
NEW: Add Preference to always write 'Album Artist' even for single artist
     albums.

0.3.10
CHG: Update repository with latest foobar2000 SDK and ensure all projects
     compile with Visual Studio 2019. No functional changes from previous
     version.

0.3.9
FIX: Repsect MusicBrainz rate limits so searches that fetch several
     releases should complete.
CHG: You can now paste full release URLs like

     https://musicbrainz.org/release/76a601d9-a1a6-4112-aad5-d63bc85db9b6

     into the AlbumID dialog and the URL will be stripped to leave
     just the MBID.

0.3.8
NEW: Add Preferences for writing release county (RELEASECOUNTRY) and
     format (MEDIA) tags.

0.3.7
CHG: Remove 99 track limit for Artist/Album and AlbumID lookups.

0.3.6
CHG: Allow TOC lookups from files with a sample rate of 48000 Hz such
     as Opus.
CHG: Enfore stricter checks on TOC submissions so only lossless files are
     supported.

0.3.5
CHG: Default to https.
CHG: Display a more helpful message when errors occur.

0.3.4
FIX: Restore support for Windows XP.

0.3.3 
NEW: Add support for setting a custom server in the Preferences.

0.3.2
CHG: foobar2000 v1.3 is the new minimum requirement.
CHG: Update repository with all pre-requisites meaning it should compile
     as-is with latest versions of foobar2000 SDK and Visual Studio.
CHG: Dialog tweaks.

0.3.1
Last release by original developer.
```
