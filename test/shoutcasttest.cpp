//#include <shoutcastmodel.h>
//#include <shoutcastview.h>
#include "shoutcastfetcher.h"
#include "shoutcaststation.h"
#include <QTest>
#include <QFile>
#include <QStringList>
#include <QSignalSpy>

class ShoutcastTest : public QObject {
	Q_OBJECT
private slots:
	void requireGenreParsingToWork();
	void requireStationListParsingToWork();
	void requireQueryingAStationURIMakesItEndUpInPendingList();
};

void ShoutcastTest::requireGenreParsingToWork() {
	QFile file(":genres.xml");
	file.open(QIODevice::ReadOnly);
	ShoutcastFetcher f;
	f.genresAvailable(&file);
	QStringList expected;
	expected << "Web" << "Whatever" << "Wir" << "Word";
	QCOMPARE(expected, f.genres());
}

void ShoutcastTest::requireStationListParsingToWork() {
	QFile file(":stations.xml");
	file.open(QIODevice::ReadOnly);
	ShoutcastFetcher f;
	f.newStationsAvailable(&file, "Rock");
	ShoutcastStationList l = f.stationsForKeyword("Rock");

	ShoutcastStation s = l.takeFirst();
	QCOMPARE(s.name(), QString(".977 The Hitz Channel"));
	QCOMPARE(s.id(), 6956);
	QCOMPARE(s.bitRate(), 128);
	QCOMPARE(s.genre(), QString("Pop Rock Top 40"));
	QCOMPARE(s.listeners(), 8483);
	QCOMPARE(s.mimeType(), QString("audio/mpeg"));
	QCOMPARE(s.currentTrack(), QString("Flo rida - Low"));
	QCOMPARE(s.tuneIn(), QString("/sbin/tunein-station.pls"));

	s = l.takeFirst();
	QCOMPARE(s.name(), QString("#MUSIK.MAIN"));
	QCOMPARE(s.id(), 491);
	QCOMPARE(s.bitRate(), 128);
	QCOMPARE(s.genre(), QString("80S POP ROCK TOP 40 WORLD"));
	QCOMPARE(s.listeners(), 6124);
	QCOMPARE(s.mimeType(), QString("audio/mpeg"));
	QCOMPARE(s.currentTrack(), QString(""));
	QCOMPARE(s.tuneIn(), QString("/sbin/tunein-station.pls"));
}

void ShoutcastTest::requireQueryingAStationURIMakesItEndUpInPendingList()
{
	ShoutcastFetcher f;
	f.fetch("Pop", QUrl(":stations.xml"));
	QSignalSpy spy(&f, SIGNAL(newStationsAvailable(const QString &)));
	int i = 1000;
	while (spy.count() == 0 && i--)
		QTest::qWait(1);
	QVERIFY(i);
	QCOMPARE(spy[0][0].toString(), QString("Pop"));
	ShoutcastStationList l = f.stationsForKeyword("Pop");
	ShoutcastStation s = l.takeFirst();
	QCOMPARE(s.name(), QString(".977 The Hitz Channel"));
}

#include "shoutcasttest.moc"
QTEST_MAIN(ShoutcastTest)
