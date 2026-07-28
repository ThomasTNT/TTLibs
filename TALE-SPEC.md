# TALE — Spezifikation (Entwurf)

**T**agged **A**… **L**anguage … **E** — Arbeitstitel; die ursprüngliche
Auflösung des Akronyms ist verloren gegangen. Vorschlag: **TA**gged
**L**ine **E**ntries.

Stand: 19.07.2026 — rekonstruiert aus `ConfigLineParser` (TTLibs/Utilities)
und `ConcertParser` (SongLivePlayer), ergänzt um die Festlegungen vom
19.07.2026.

---

## 1. Einführung und Ziele

TALE ist eine zeilenbasierte Konfigurationssprache für hierarchische Daten.
Sie ist für den Menschen gut les- und schreibbar (Konzertdateien werden vor
Auftritten von Hand editiert) und für Parser mit minimalem Aufwand in einem
Durchlauf lesbar.

Abgrenzung zu YAML:

1. **Einrückung hat keine Bedeutung.** Sie dient ausschließlich der Optik.
   Die Hierarchie entsteht aus der Reihenfolge der Tags und den Regeln in
   Abschnitt 6 (bzw. aus dem Schema).
2. **Tags können Werte tragen, auch vektoriell:** `gain = 0.2, 0.4`.
3. Ein Tag kann **gleichzeitig** einen Wert tragen und Kind-Tags besitzen
   (`metronom = yes` mit Kind `metronomout = 2`) — das kann YAML nicht.
4. Aufzählungen entstehen durch **nummerierte Tags** (`song1`, `song2`, …)
   statt durch Sequenz-Syntax.
5. **Rohtabellen-Blöcke:** frei formatierte, spaltenorientierte Datenzeilen
   zwischen einem Tag und seinem Endtag (`tempolist` … `tempolistend`).
6. TALE kennt ein eingebautes **Schema-Konzept** (Pendant zur XSD);
   YAML validiert Struktur nicht selbst.

---

## 2. Lexikalische Struktur

* Ein TALE-Dokument ist eine Folge von **Zeilen**. Jede Zeile wird für sich
  verarbeitet; es gibt keine zeilenübergreifenden Konstrukte.
* **Kommentare** beginnen mit `#` und reichen bis zum Zeilenende. Sie können
  allein auf einer Zeile stehen oder einem Inhalt folgen. Ein `#` innerhalb
  eines gequoteten Wertes (Abschnitt 4) beginnt keinen Kommentar.
* Nach Entfernen des Kommentars wird die Zeile **getrimmt** (Whitespace an
  Anfang und Ende entfällt). **Leere Zeilen werden ignoriert.**
* **Einrückung ist bedeutungslos** (Grundprinzip 1).
* **Namen** (Identifier) beginnen mit einem Buchstaben und bestehen aus
  Buchstaben und Ziffern. Groß-/Kleinschreibung ist signifikant
  (`thresholdOn`).

### Grammatik (EBNF-Skizze)

```ebnf
dokument    = { zeile } ;
zeile       = [ ws ] , [ inhalt ] , [ ws ] , [ kommentar ] , zeilenende ;
kommentar   = "#" , { beliebiges-zeichen-ausser-zeilenende } ;
inhalt      = tagzeile | datenzeile ;
tagzeile    = name , [ ws ] , [ "=" , [ ws ] , [ wert ] ] ;
name        = buchstabe , { buchstabe | ziffer } ;
wert        = quotwert | werteliste ;
quotwert    = '"' , beliebiger-text , '"' ;      (* siehe Abschnitt 4 *)
werteliste  = element , { "," , element } ;
element     = getrimmter-rohtext ;               (* ohne "," und "#" *)
datenzeile  = zeile-die-nicht-mit-buchstabe-beginnt ;
```

Ob eine `tagzeile` einen Abschnitt öffnet (nacktes Tag) oder ein Wert-Tag
ist (`name = …`), entscheidet die Form. Ob ein Name eine Instanznummer
trägt (`song12`), entscheiden die Regeln in Abschnitt 5 bzw. das Schema —
das ist eine **semantische**, keine lexikalische Unterscheidung.

---

## 3. Zeilenformen

| Form | Beispiel | Bedeutung |
|---|---|---|
| Abschnitts-Tag | `audio` | öffnet einen Abschnitt |
| Wert-Tag | `samplerate = 44100` | Schlüssel-Wert-Paar |
| Nummeriertes Tag | `song1`, `wave2 = datei.wav` | Instanz einer Aufzählung, mit oder ohne Wert |
| Endtag | `tempolistend` | schließt den Abschnitt `tempolist` explizit |
| Datenzeile | `1   80   4/4` | Rohdaten des aktuell offenen Abschnitts |

Ein **Endtag** ist ein Tag, dessen Name aus dem Namen eines offenen
Abschnitts plus dem Suffix `end` besteht. Endtags sind für jeden Abschnitt
erlaubt (optional); **erforderlich** sind sie nur für Rohblöcke, deren
Datenzeilen sonst kein Ende hätten. Ein Endtag schließt den benannten
Abschnitt **mitsamt allen noch offenen Nachfahren** (siehe Abschnitt 6).

Eine **Datenzeile** ist jede Inhaltszeile, die nicht mit einem Buchstaben
beginnt. Sie gehört als Rohdatum zum innersten offenen Abschnitt. Die
Interpretation der Spalten (Whitespace-getrennt) ist Sache der Anwendung
bzw. des Schemas. Empfehlung: Datenzeilen sollten mit einer Zahl beginnen,
damit sie auch beim schemalosen Parsen eindeutig als Daten erkennbar sind.

---

## 4. Werte und Typen

* Der Wert steht nach dem Zuweisungszeichen **`=`** (das einzige;
  `:` ist reserviert und wird nicht verwendet).
* Werte sind **Rohtext bis zum Zeilenende** (abzüglich Kommentar),
  getrimmt. Sie dürfen Leerzeichen, Backslashes usw. enthalten.
* **Leere Werte** sind erlaubt (`connect =`) und bedeuten leerer String
  bzw. leerer Vektor.
* **Vektoren:** Kommata trennen Elemente: `trackout = 1, 2`. Jedes Element
  wird getrimmt. Beim schemalosen Parsen macht ein Komma den Wert zum
  Vektor; mit Schema legt das Schema fest, ob skalar oder vektoriell.
* Skalare Typen (durch Schema oder Anwendung festgelegt):
  * Ganzzahl (`int`, `uint`), Gleitkommazahl (Dezimal**punkt**)
  * String
  * Bool: `yes` / `no`
* Ohne Typinformation ist jeder Wert ein String.

### Quoting

Ein skalarer Wert darf in `"…"` eingeschlossen werden. Dann gilt:

* Es zählen **die beiden äußersten** Anführungszeichen: Der Wert ist
  alles zwischen dem ersten und dem letzten `"` der Zeile. Alles
  dazwischen ist echt — es gibt **kein Escaping**:
  `name = ""foo""` ergibt `"foo"` (nicht `foo`).
* `#` und `,` verlieren innerhalb des Quotings ihre Sonderbedeutung.
* Nach dem schließenden Anführungszeichen dürfen nur Whitespace und ein
  Kommentar folgen; dieser Kommentar darf **kein `"` enthalten** (sonst
  verschöbe sich das „letzte" Anführungszeichen).
* Quoting gilt nur für **skalare** Werte. Vektorelemente sind rohe,
  kommafreie Texte — Listen von Strings mit Kommata bildet man über
  TALEs natives Aufzählungsmittel ab: nummerierte Tags
  (`name1 = …`, `name2 = …`).

---

## 5. Nummerierte Tags (Aufzählungen)

* Eine **Aufzählung** ist eine Gruppe von Geschwister-Tags mit gleichem
  Namensstamm und angehängter Dezimalnummer: `song1`, `song2`, … `songN`.
* Tagnamen **dürfen Ziffern enthalten**. Ob eine Ziffernfolge am Namensende
  eine Instanznummer ist, entscheidet sich schemalos über die
  **Schwester-Regel:** Gibt es ein Geschwister-Tag mit gleichem Stamm und
  anderer Nummer, wird die Gruppe als Aufzählung (Vektor) erkannt. Ein
  einzelnes `foo3` ohne Schwestern ist ein gewöhnlicher Tagname.
* **Nummern sind bei Aufzählungen Pflicht.** Es gibt keinen Rückfall auf 0.
  Existieren nummerierte Schwestern, ist ein unnummeriertes Tag gleichen
  Stamms **ungültig** (Fehler).
* Diese Regeln gelten für das schemalose Parsen. Mit Schema legt das
  Schema fest, welche Tags Aufzählungen sind.

---

## 6. Hierarchie

Ein Dokument ist ein Baum. Abschnitts-Tags öffnen Knoten; Wert-Tags und
Datenzeilen sind Blätter bzw. Nutzlast.

### Eindeutigkeitsregeln

Mehrdeutigkeiten sind verboten:

* Ein Tag darf keinen **gleichnamigen Vorfahren** haben — unter `foo`
  darf weder direkt noch tiefer wieder ein `foo` vorkommen. (Gleiche
  Namen in verschiedenen Zweigen sind erlaubt: `name` darf unter
  `concert`, `song` und `track` jeweils vorkommen.)
* Kein Tag darf so heißen wie „Name eines offenen Abschnitts + `end`" —
  sonst wäre es nicht vom Endtag unterscheidbar.
* Namen von Abschnitten auf oberster Ebene („globale Tags") sind
  reserviert: Kein inneres Tag darf so heißen.

### Einordnung und implizites Schließen

Der Text kodiert die Verschachtelungstiefe nicht lexikalisch (keine
Einrückungssemantik, Endtags optional). Es gelten der Reihe nach:

1. **Kind-Konvention:** Ein nacktes Tag wird Kind des innersten offenen
   Abschnitts (maximal tiefe Einordnung).
2. **Schwester-Regel:** Stimmt der Name (bzw. Namensstamm bei
   Aufzählungen) mit einem offenen Abschnitt überein, schließt das Tag
   alle Abschnitte bis zu dessen Ebene und wird sein Geschwister
   (`song2` schließt `track1`, `metronom` … und `song1` selbst).
   Dank der Eindeutigkeitsregeln ist das deterministisch.
3. **Endtag:** `<name>end` schließt den Abschnitt `name` mitsamt allen
   noch offenen Nachfahren — in einem Schritt.

### Rücksprung zu einem äußeren Elternteil

Die Folge `tag1` → `tag2` → `tag3` → `tag4`, bei der `tag4` ein Kind von
`tag1` (parallel zu `tag2`) sein soll, ist **aus dem Text allein nicht
ableitbar** — die Kind-Konvention würde `tag4` unter `tag3` einhängen.
Es gibt zwei Lösungen:

* **Kontext (Schema):** Das Schema kennt die Elternkette von `tag4`;
  beim seriellen Lesen wird der offene Pfad auf die deklarierte Tiefe
  zurückgestutzt. So arbeitet die Referenzimplementierung
  (`CheckTagHierachySuitable`), z. B. bei `metronom` nach `track1`.
* **Endtag (schemalos):** Eine Zeile genügt, da Endtags Nachfahren
  mitschließen:

  ```
  tag1
    tag2
      tag3
    tag2end      # schliesst tag3 UND tag2
    tag4         # Kind von tag1, parallel zu tag2
  ```

**Writer-Regel:** Ein schema-kundiger Writer soll Endtags überall dort
ausgeben, wo der nächste Knoten flacher liegt, als die Kind-Konvention
ergäbe. Solche Dokumente parsen auch schemalos zum korrekten Baum —
das Schema ist dann Optimierung, nicht Voraussetzung.

---

## 7. Parse-Modi

### 7.1 Mit Schema

* Das Schema (Abschnitt 8) definiert Pfade, Typen, Aufzählungen und
  Rohblöcke. Die Hierarchie folgt vollständig dem Schema.
* **Unbekannte Tags sind Fehler.** Dieses Verhalten ist abschaltbar
  (lenient-Modus: Warnung statt Fehler); stumm verschluckt wird nie —
  jede unbekannte oder unplatzierte Zeile wird mindestens mit
  Zeilennummer gemeldet.

### 7.2 Ohne Schema (generisches Parsen)

* Alle Tags werden mitgeparst und nach den Regeln der Abschnitte 5 und 6
  in einen generischen Baum eingeordnet.
* Der generische Baum bewahrt **Reihenfolge und Kommentare**, sodass
  Lesen → Schreiben verlustfrei ist (wichtig für Werkzeuge wie Editoren
  und Pretty-Printer).
* Grenzen des schemalosen Modus: Ohne Endtags ist die Baumform beim
  Rücksprung-Fall (Abschnitt 6) eine Näherung; ein Wert-Tag mit Kindern
  (`metronom = yes` + `metronomout`) wird als Blatt plus Geschwister
  gelesen; Datenzeilen, die mit einem Buchstaben beginnen, würden als
  Tags gelesen. Alles davon ist beim Zurückschreiben verlustfrei.

---

## 8. Schemasprache (Entwurf, Pendant zur XSD)

Das Schema beschreibt jedes Tag mit seinem Pfad und seinen Eigenschaften.
In der Referenzimplementierung ist das Schema heute in C++ kodiert
(Elternkette `parent0…parent3` je Parse-Aufruf); die Fehlermeldungen
nutzen bereits die Pfadnotation `eltern->kind`.

Je Tag legt das Schema fest:

* **Pfad:** `global->audio->samplerate` (Wildcard `*` für eine Ebene
  ist vorgesehen).
* **Art:** Abschnitt | Wert-Tag | Rohblock (mit Endtag-Pflicht).
* **Typ** des Wertes: int | uint | double | string | bool — jeweils
  skalar oder Vektor.
* **Aufzählung:** ja/nein (nummerierte Instanzen).
* **Matching-Modus:**
  * **exakt** — der Pfad muss vollständig und in exakter Tiefe stimmen
    (deklarative Sicht, „XSD-artig"); typisch für Blatt-Tags.
  * **seriell** — das Tag darf tiefer im Baum angetroffen werden und
    schließt implizit alle tieferen offenen Abschnitte; typisch für
    Abschnitts-Tags (Positionsprinzip wie im `ConcertParser`).

Die maximale Verschachtelungstiefe ist **unbegrenzt**; die Grenze von
vier Eltern in der Referenzimplementierung ist eine Implementierungs-,
keine Sprachgrenze.

Eine textuelle Schema-Notation (Schema-Dateien statt C++-Deklaration)
ist noch zu entwerfen; naheliegend ist, dafür TALE selbst zu verwenden.

---

## 9. Parser-Architektur (Empfehlung)

Drei Schichten, analog zur YAML-Verarbeitungskette
(Events → Knotengraph → native Objekte), aber mit eingebauter Validierung:

1. **Ereignis-Schicht (zeilenweise):** Liest das Dokument in einem
   Durchlauf und liefert Ereignisse: *Abschnitt geöffnet (Name, Nummer)*,
   *Wert (Name, Nummer, Werte)*, *Datenzeile (Rohtext)*, *Abschnitt
   geschlossen*. Schemafrei, minimaler Speicherbedarf. Anwendungen dürfen
   direkt auf dieser Schicht arbeiten — der heutige `ConcertParser` ist
   ein gültiger Parser dieser Klasse.
2. **Dokumentbaum (DOM):** Generischer, reihenfolge- und
   kommentarerhaltender Baum, gebaut aus den Ereignissen — mit Schema
   exakt, ohne Schema nach den Regeln aus Abschnitt 7.2. Grundlage für
   Werkzeuge (Editor, Diff, Pretty-Printer, Konvertierung).
3. **Schema-Schicht:** Validiert streamend (auf Schicht 1) oder am Baum
   (auf Schicht 2); Fehlerpolitik strict/lenient wie in Abschnitt 7.1.

---

## 10. Offene Fragen

* **Akronym:** endgültige Auflösung von „TALE" festlegen.
* **Zeichencodierung:** Bestandsdateien sind Latin-1 (Umlaute);
  für die Veröffentlichung UTF-8 festlegen?
* **Versionskennung:** z. B. `tale = 1` als erste Zeile, damit alte
  Parser neue Dokumente erkennen können (mildert das Dilemma
  „unbekanntes Tag: Tippfehler oder neuere Schema-Version?").
* **Unterstrich in Namen:** bisher ungenutzt — erlauben?
* **Schema-Notation:** textuelles Schemaformat (in TALE selbst?)
  entwerfen.
* **Referenzimplementierung:** `ConfigLineParser` auf die
  Ereignis-Schicht heben (Nummernpflicht statt Rückfall auf 0,
  Meldung statt stummem Ignorieren, quote-bewusste Kommentar-Entfernung);
  Debug-Rest in `ConcertParser.cpp` (Zeile ~150, `linecount == 112`)
  entfernen.
