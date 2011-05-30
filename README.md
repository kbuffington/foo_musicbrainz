foo_musicbrainz
===============

**foo_musicbrainz** is a component for [foobar2000](http://www.foobar2000.org/) media player. It can tag files by fetching data from [MusicBrainz](http://musicbrainz.org/).

Some notable features:

* Fetches data by automatically calculated MusicBrainz Disc ID, existing Artist/Album or MusicBrainz Album ID tags;
* Uses the latest version of [MusicBrainz web service](http://musicbrainz.org/doc/XML_Web_Service/Version_2), which corresponds to the [NGS](http://musicbrainz.org/doc/Next_Generation_Schema) data model;
* Supports many different tags, including artist, album, date, tracknumber/totaltracks, discnumber/totaldiscs, label, catalognumber, barcode etc.;
* Supports MusicBrainz-specific tags: Artist ID, Album ID, Release Group ID, Disc ID, Track ID, album type, album status;
* Missing TOCs can be submitted to MusicBrainz from the context menu.

Links
-----

* [Download stable build](http://www.foobar2000.org/components/view/foo_musicbrainz)
* [Discuss on Hydrogenaudio Forums](http://www.hydrogenaudio.org/forums/index.php?showtopic=70623)
* [Join Last.fm group](http://www.last.fm/group/foo_musicbrainz)

Building the code
-----------------

You should have Visual Studio 2010 to build this component. Also, you will need to get two dependencies:

* [foobar2000 SDK](http://www.foobar2000.org/SDK)
* [TinyXML](http://sourceforge.net/projects/tinyxml/files/tinyxml/)

Both dependencies and foo_musicbrainz should have a single parent directory, so the catalog structure will be similar to this:

* Projects
  * foo_musicbrainz
  * foobar2000
  * pfc _(included with the foobar2000 SDK)_
  * tinyxml

Download archives with dependencies and extract their contents to the desired directory. From the same directory, type in your console:

    git clone git://github.com/Dremora/foo_musicbrainz.git

Voilà! Now you can open foo_musicbrainz\foo_musicbrainz.sln and build the component!
