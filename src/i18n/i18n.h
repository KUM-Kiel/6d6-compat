// This file was automatically generated.
// Do not edit!
#ifndef I18N_HEADER
#define I18N_HEADER
typedef struct {
  const char *comment_too_long;
  const char *could_not_create_file_ss;
  const char *could_not_open_logfile_s;
  const char *could_not_open_ss;
  const char *could_not_restore_uid;
  const char *created_file_s;
  const char *errno_eacces;
  const char *errno_eagain;
  const char *errno_edquot;
  const char *errno_eexist;
  const char *errno_einval;
  const char *errno_enoent;
  const char *gain;
  const char *invalid_channel_names;
  const char *invalid_cut;
  const char *invalid_end_time;
  const char *invalid_output_template_c;
  const char *invalid_start_time;
  const char *invalid_station_code;
  const char *io_error;
  const char *io_error_d;
  const char *label_6d6_sn;
  const char *label_blank;
  const char *label_channels;
  const char *label_comment;
  const char *label_duration;
  const char *label_end_time;
  const char *label_sample_rate;
  const char *label_size;
  const char *label_skew;
  const char *label_skew_time;
  const char *label_start_time;
  const char *label_sync_time;
  const char *leapsec_outdated;
  const char *lost_samples_d_ll;
  const char *malformed_6d6_header;
  const char *need_argument_long_s;
  const char *need_argument_short_c;
  const char *need_name_for_every_channel;
  const char *out_of_memory;
  const char *processing_s;
  const char *skew_ignored_warning;
  const char *too_many_channel_names;
  const char *unexpected_argument_s;
  const char *unrecognised_long_s;
  const char *unrecognised_short_c;
  const char *usage_6d6copy_s;
  const char *usage_6d6info_s;
  const char *usage_6d6mseed_s;
  const char *usage_6d6read_s;
  const char *using_channel_mapping;
  const char *version_ss;
} I18n;
extern const I18n *en_GB;
extern const I18n *de_DE;
extern const I18n *i18n;
static inline void i18n_set_lang(const char *lang)
{
  if (!lang) {
    i18n = en_GB;
  } else if ((lang[0] == 'E' || lang[0] == 'e') && (lang[1] == 'N' || lang[1] == 'n') && lang[2] == '_' && (lang[3] == 'G' || lang[3] == 'g') && (lang[4] == 'B' || lang[4] == 'b')) {
    i18n = en_GB;
  } else if ((lang[0] == 'D' || lang[0] == 'd') && (lang[1] == 'E' || lang[1] == 'e') && lang[2] == '_' && (lang[3] == 'D' || lang[3] == 'd') && (lang[4] == 'E' || lang[4] == 'e')) {
    i18n = de_DE;
  } else if ((lang[0] == 'E' || lang[0] == 'e') && (lang[1] == 'N' || lang[1] == 'n')) {
    i18n = en_GB;
  } else if ((lang[0] == 'D' || lang[0] == 'd') && (lang[1] == 'E' || lang[1] == 'e')) {
    i18n = de_DE;
  } else {
    i18n = en_GB;
  }
}
#endif

#ifdef I18N_IMPLEMENTATION
#undef I18N_IMPLEMENTATION
static I18n _en_GB = {
  .comment_too_long = "The comment is too long to fit into the header.\n",
  .could_not_create_file_ss = "Could not create file '%s': %s.\n",
  .could_not_open_logfile_s = "Could not open logfile: %s.\n",
  .could_not_open_ss = "Could not open '%s': %s.\n",
  .could_not_restore_uid = "Error while restoring user id.\n",
  .created_file_s = "Created file '%s'.\n",
  .errno_eacces = "Permission denied",
  .errno_eagain = "The operation would block",
  .errno_edquot = "Quota exhausted",
  .errno_eexist = "The target already exists",
  .errno_einval = "Invalid argument",
  .errno_enoent = "No such file or directory",
  .gain = "Gain",
  .invalid_channel_names = "Channel names must be between 1 and 3 alphanumeric characters.\n",
  .invalid_cut = "Invalid value for '--cut'.\n",
  .invalid_end_time = "Invalid value for '--end-time'.\n",
  .invalid_output_template_c = "Error in output template: %%%c is not a valid placeholder!\n",
  .invalid_start_time = "Invalid value for '--start-time'.\n",
  .invalid_station_code = "Please specify a station code of 1 to 5 alphanumeric characters with --station=CODE.\n",
  .io_error = "I/O error!\n",
  .io_error_d = "I/O error (%d)!\n",
  .label_6d6_sn = "    6D6 S/N:",
  .label_blank = "           :",
  .label_channels = "   Channels:",
  .label_comment = "    Comment:",
  .label_duration = "   Duration:",
  .label_end_time = "   End Time:",
  .label_sample_rate = "Sample Rate:",
  .label_size = "       Size:",
  .label_skew = "       Skew:",
  .label_skew_time = "  Skew Time:",
  .label_start_time = " Start Time:",
  .label_sync_time = "  Sync Time:",
  .leapsec_outdated = "\n############################################################\n#                     !!! WARNING !!!                      #\n#         The leapsecond information is outdated.          #\n#         Please download the newest release here:         #\n#      https://github.com/KUM-Kiel/6d6-compat/releases     #\n############################################################\n\n",
  .lost_samples_d_ll = "Lost %d samples at file position 0x%llx.\n",
  .malformed_6d6_header = "Malformed 6D6 header!\n",
  .need_argument_long_s = "Option '--%s' needs an argument.\n",
  .need_argument_short_c = "Option '-%c' needs an argument.\n",
  .need_name_for_every_channel = "When specifying channel names, you must give a name for every channel.\n",
  .out_of_memory = "Out of memory!\n",
  .processing_s = "Processing '%s'.\n",
  .skew_ignored_warning = "Warning: You have chosen to deactivate the skew correction.\nThis is almost certainly a bad idea unless you know exactly what you want\nto achieve.\n",
  .too_many_channel_names = "Too many channel names.\n",
  .unexpected_argument_s = "Option '--%s' expects no argument.\n",
  .unrecognised_long_s = "Unrecognised option '--%s'.\n",
  .unrecognised_short_c = "Unrecognised option '-%c'.\n",
  .usage_6d6copy_s = "6d6copy\n=======\n\nThe program '6d6copy' makes a perfect copy of a StiK or 6D6 SD card\nto a file.\nThis is a great backup mechanism and you can use the .6d6 files as a\nstarting point for arbitrary data analysis methods.\n\nUsage\n-----\n\n  %s [options] /dev/sdX out.6d6\n\nThe first argument is the source of the data. This is normally your\nStiK or SD card device like '/dev/sdb' or '/dev/mmcblk0'.\nThe second argument is the file to which the copy will be made.\nThis file should have a .6d6 ending to identify it as 6D6 raw data.\n\nWhen you start the program and the input and output files are valid,\nthe copy operation begins and the progress is shown on the terminal.\nTo suppress that progress display you can use the flags '-q' or\n'--no-progress'. This might be useful in automated scripts.\n\nOptions\n-------\n\n--append-comment=TEXT\n\n  Append the given text to the comment header. If the text contains a backslash\n  it escapes the next character. You can write \\n to insert a newline and \\\\ to\n  insert a literal backslash.\n\n-q\n--no-progress\n\n  Suppresses the progress display.\n\n--json-progress\n\n  Output progress in form of JSON messages. Each message is on one line.\n  This makes the progress output got to STDOUT instead of STDERR.\n\nExamples\n--------\n\nArchive the StiK in '/dev/sdb' to a backup hard drive.\n\n  $ 6d6copy /dev/sdb /media/Backup/Experiment-003/Station-007.6d6\n\nCopy the SD card in '/dev/mmcblk0' to the current directory.\n\n  $ 6d6copy /dev/mmcblk0 Station-013.6d6\n\n",
  .usage_6d6info_s = "6d6info\n=======\n\nThe program '6d6info' shows some information about a 6D6 recording.\nYou can either use it on a StiK or SD card or to get some information\nabout a .6d6 file on your hard drive.\n\nUsage\n-----\n\n  %s [options] /dev/sdX\n\nThe parameter is the file or device you want to inspect.\n\nOptions\n-------\n\n--json\n\n  Output JSON instead of formatted text.\n\nExamples\n--------\n\nInspect the StiK in '/dev/sdb':\n\n  $ 6d6info /dev/sdb\n\nInspect the file 'station-007.6d6' in the directory 'line-001':\n\n  $ 6d6info line-001/station-007.6d6\n\n",
  .usage_6d6mseed_s = "6d6mseed\n========\n\nThe program '6d6mseed' is used to convert raw data from the 6D6 datalogger\ninto the MiniSEED format.\n\nUsage\n-----\n\n  %s [options] input.6d6\n\nOptions\n-------\n\n--station=CODE\n\n  Set the MiniSEED station code to CODE. The station code is required for\n  MiniSEED generation. It can contain between one and five alphanumeric\n  ASCII characters.\n\n--location=CODE\n\n  Set the location to CODE. This should usually be a two character code.\n\n--network=CODE\n\n  Set the network code to CODE. This is a two character code assigned by IRIS.\n\n--channels=C1,C2,...\n\n  Rename the channels. When using this option, you have to give a new name for\n  every channel which is between 1 and 3 alphanumeric characters. The names\n  must be seperated by commas without spaces.\n\n--output=FILENAME_TEMPLATE\n\n  Set a template for output files. The template string may contain the following\n  placeholders:\n\n    %%y - Year\n    %%m - Month\n    %%d - Day\n    %%h - Hour\n    %%i - Minute\n    %%s - Second\n    %%S - Station Code\n    %%L - Location\n    %%C - Channel\n    %%N - Network\n    %%j - Julian Day\n\n  The default value is 'out/%%S/%%y-%%m-%%d-%%C.mseed'.\n\n--cut=SECONDS\n\n  Cut the data in files of SECONDS. The default value is 86400, i.e. one day.\n\n--no-cut\n\n  Do not cut the MiniSEED files and produce one big file per channel.\n\n--start-time=TIME\n--end-time=TIME\n\n  Limit the conversion to the time between start and end.\n  If these options are used, the resulting MiniSEED files will not include any\n  samples before the given start time and no samples after the given end time.\n\n  The default values are the beginning and ending of the recording.\n\n  The times must be given as ISO string, i.e. 'YYYY-MM-DDTHH:MM:SSZ'.\n\n--logfile=FILE\n\n  Create a logfile at FILE.\n  The logfile contains all important information regarding recording data\n  and created files. It also lists errors which occured during processing.\n\n--auxfile=FILE\n\n  Create a CSV file with auxiliary data at FILE.\n  The auxfile contains engineering data like the temperature and battery\n  voltage.\n\n--ignore-skew\n\n  Do not use the second synchronisation. This will on average double the timing\n  error and thus will always print a warning and should not be used.\n\n--resample\n\n  Do skew correction by resampling the data to the nominal sampling frequency.\n\nExamples\n--------\n\nConvert the file 'ST007.6d6' to MiniSEED using default values.\n\n  $ 6d6mseed --station=ST007 ST007.6d6\n\nConvert the file 'ST007.6d6' specifying everything.\n\n  $ 6d6mseed ST007.6d6 \\\n      --station=ST007 \\\n      --network=XX \\\n      --output=%%N/%%S/%%y-%%m-%%d-%%C.mseed\n\n",
  .usage_6d6read_s = "6d6read\n=======\n\nThe program '6d6read' is used to convert raw data from the 6D6 datalogger\ninto the Send2X format.\nThis is useful if you still have a lot of old dataloggers and want to use\na uniform method of data analysis.\n\nUsage\n-----\n\n  %s [-q|--no-progress] < in.6d6 > out.s2x\n\nThe input file is a .6d6 file and is written with an opening angle bracket '<'.\nThe output file will be a .s2x file and is preceded by a closing angle\nbracket '>'.\n\nIf the input file is a valid .6d6 file, the conversion process will begin and\nthe progress will be reported to the terminal. If you want to suppress the\nprogress display, you can use the flag '-q' or '--no-progress'. This might be\nuseful in automated scripts.\n\nExamples\n--------\n\nConvert the file 'station-007.6d6' to Send2X format:\n\n  $ 6d6read < station-007.6d6 > station-007.s2x\n\nConvert the file 'x.6d6' to 'y.s2x' and suppress the progress display:\n\n  $ 6d6read --no-progress < x.6d6 > y.s2x\n",
  .using_channel_mapping = "Using Channel Mapping:\n",
  .version_ss = "6D6 Compatibility Tools Version %s (%s)\n\n",
};
const I18n *en_GB = &_en_GB;
static I18n _de_DE = {
  .comment_too_long = "Der Kommentar passt nicht in den 6D6-Header.\n",
  .could_not_create_file_ss = "Konnte Datei '%s' nicht erstellen: %s.\n",
  .could_not_open_logfile_s = "Konnte Logdatei nicht öffnen: %s.\n",
  .could_not_open_ss = "Konnte '%s' nicht öffnen: %s.\n",
  .could_not_restore_uid = "Beim Wiederherstellen der User-ID ist ein Fehler aufgetreten.\n",
  .created_file_s = "Datei '%s' erstellt.\n",
  .errno_eacces = "Zugriff verweigert",
  .errno_eagain = "Die Operation würde blockieren",
  .errno_edquot = "Das Kontingent wurde ausgeschöpft",
  .errno_eexist = "Das Ziel existiert bereits",
  .errno_einval = "Ungültiges Argument",
  .errno_enoent = "Die Datei oder das Verzeichnis existiert nicht",
  .gain = "Gain",
  .invalid_channel_names = "Kanalnamen müssen aus 1 bis 3 alphanumerischen Zeichen bestehen.\n",
  .invalid_cut = "Ungültiger Wert für '--cut'.\n",
  .invalid_end_time = "Ungültiger Wert für '--end-time'.\n",
  .invalid_output_template_c = "Fehler im Output-Template: %%%c ist kein gültiger Platzhalter!\n",
  .invalid_start_time = "Ungültiger Wert für '--start-time'.\n",
  .invalid_station_code = "Bitte geben Sie einen Stationscode von 1 bis 5 alphanumerischen Zeichen mit\n--station=CODE an.\n",
  .io_error = "I/O-Fehler!\n",
  .io_error_d = "I/O-Fehler (%d)!\n",
  .label_6d6_sn = "    6D6 S/N:",
  .label_blank = "           :",
  .label_channels = "     Kanäle:",
  .label_comment = "  Kommentar:",
  .label_duration = "      Dauer:",
  .label_end_time = "    Endzeit:",
  .label_sample_rate = " Abtastrate:",
  .label_size = " Dateigröße:",
  .label_skew = "       Skew:",
  .label_skew_time = "  Skew-Zeit:",
  .label_start_time = "  Startzeit:",
  .label_sync_time = "  Sync-Zeit:",
  .leapsec_outdated = "\n############################################################\n#                     !!! WARNUNG !!!                      #\n#  Die Schaltsekundeninformation ist nicht mehr aktuell.   #\n#    Bitte laden Sie die neuste Version hier herunter:     #\n#     https://github.com/KUM-Kiel/6d6-compat/releases      #\n############################################################\n\n",
  .lost_samples_d_ll = "%d verlorene Samples an Position 0x%llx.\n",
  .malformed_6d6_header = "Beschädigter 6D6-Header!\n",
  .need_argument_long_s = "Die Option '--%s' benötigt ein Argument.\n",
  .need_argument_short_c = "Die Option '-%c' benötigt ein Argument.\n",
  .need_name_for_every_channel = "Es muss für jeden Kanal ein Name angegeben werden.\n",
  .out_of_memory = "Arbeitsspeicher ist voll!\n",
  .processing_s = "Verarbeite '%s'.\n",
  .skew_ignored_warning = "Warnung: Die Skew-Korrektur ist ausgeschaltet.\nDies ist in fast allen Fällen eine schlechte Idee und sollte nur verwendet\nwerden, wenn man genau weiß, was man erreichen möchte.\n",
  .too_many_channel_names = "Zu viele Kanalnamen angegeben.\n",
  .unexpected_argument_s = "Die Option '--%s' benötigt kein Argument.\n",
  .unrecognised_long_s = "Unbekannte Option '--%s'.\n",
  .unrecognised_short_c = "Unbekannte Option '-%c'.\n",
  .usage_6d6copy_s = "6d6copy\n=======\n\nDas Programm '6d6copy' erstellt eine 1:1-Kopie eines StiKs oder einer SD-Karte\nin eine .6d6-Datei.\nEine .6d6-Datei ist besonders für Backups geeignet, da sie alle Rohdaten in\nunveränderter Form enthält. Sie ist somit auch prädestinierter Startpunkt für\nbeliebige Datenanalysen und Konvertierungen.\n\nVerwendung\n----------\n\n  %s [Optionen] /dev/sdX out.6d6\n\nDer erste Parameter ist die Quelldatei. Dies ist normalerweise der Gerätepfad\ndes StiKs oder der SD-Karte und sieht ähnlich aus wie '/dev/sdb' oder\n'/dev/mmcblk0'.\nDer zweite Parameter ist der Dateiname, unter dem die Kopie gespeichert werden\nsoll. Diese Datei sollte am besten die Endung '.6d6' haben, um sie als\n6D6-Rohdaten-Datei zu identifizieren.\n\nWenn Eingabe- und Ausgabedatei gültig sind, beginnt der Kopiervorgang und der\nFortschritt wird im Terminal angezeigt. Um die Ausgabe z.B. für automatisierte\nScripts zu unterdrücken, kann die Option '-q' bzw. '--no-progress' verwendet\nwerden.\n\nOptionen\n--------\n\n--append-comment=TEXT\n\n  Hängt den angegebenen Text an den Kommentar-Header an. Wenn der Text\n  Backslashes enthält, wird das nächste Zeichen speziell behandelt.\n  Man kann \\n schreiben, um einen Zeilenumbruch zu erzeugen und \\\\ für einen\n  Backslash.\n\n-q\n--no-progress\n\n  Unterdrückt die Fortschrittsanzeige.\n\n--json-progress\n\n  Die Fortschrittsanzeige erfolt in Form von JSON-Nachrichten. Jede Nachricht\n  wird durch einen Zeilenumbruch abgeschlossen. Die Ausgabe erfolgt nach STDOUT\n  statt nach STDERR.\n\nBeispiele\n---------\n\nArchiviert den StiK in '/dev/sdb' auf eine Backup-Festplatte.\n\n  $ 6d6copy /dev/sdb /media/Backup/Experiment-003/Station-007.6d6\n\nKopiert die SD-Karte in '/dev/mmcblk0' ins aktuelle Verzeichnis.\n\n  $ 6d6copy /dev/mmcblk0 Station-013.6d6\n\n",
  .usage_6d6info_s = "6d6info\n=======\n\nDas Programm '6d6info' zeigt Informationen zu einer 6D6-Aufnahme an.\nEs kann entweder direkt mit einem StiK oder einer SD-Karte verwendet werden oder\nInformationen über eine .6d6-Datei auf der Festplatte anzeigen.\n\nVerwendung\n----------\n\n  %s [Optionen] /dev/sdX\n\nDer Parameter ist die Datei oder das Gerät, das untersucht werden soll.\n\nOptionen\n--------\n\n--json\n\n  Gibt JSON statt formatiertem Text aus.\n\nBeispiele\n---------\n\nZeigt Informationen zum StiK in '/dev/sdb':\n\n  $ 6d6info /dev/sdb\n\nZeigt Informationen zur Datei 'station-007.6d6' im Ordner 'line-001':\n\n  $ 6d6info line-001/station-007.6d6\n\n",
  .usage_6d6mseed_s = "6d6mseed\n========\n\nDas Programm '6d6mseed' konvertiert Rohdaten des 6D6-Datenloggers in das\nMiniSEED-Format.\n\nVerwendung\n----------\n\n  %s [Optionen] input.6d6\n\nOptionen\n--------\n\n--station=CODE\n\n  Setzt den MiniSEED-Stationscode auf CODE. Der Stationscode ist für die\n  MiniSEED-Erzeugung zwingend erforderlich. Er besteht aus 1 bis 5 alpha-\n  numerischen ASCII-Zeichen.\n\n--location=CODE\n\n  Setzt den Location-Code auf CODE. Der Location-Code besteht aus zwei Zeichen\n  und kann benutzt werden um mehrere Sensoren an einer Station zu unterscheiden.\n\n--network=CODE\n\n  Setzt den Network-Code auf CODE. Dies ist ein zweistelliger Code, der vom\n  IRIS vergeben wird.\n\n--channels=C1,C2,...\n\n  Benutzt abweichende Kanalnamen. Die Namen müssen aus 1 bis 3 alphanumerischen\n  Zeichen bestehen und werden durch Kommata ohne Leerzeichen getrennt.\n  Wenn diese Option benutzt wird, muss für jeden Kanal ein Name festgelegt\n  werden.\n\n--output=FILENAME_TEMPLATE\n\n  Setzt ein Muster für Dateinamen. Im Muster können die folgenden Platzhalter\n  verwendet werden:\n\n    %%y - Jahr\n    %%m - Monat\n    %%d - Tag\n    %%h - Stunde\n    %%i - Minute\n    %%s - Sekunde\n    %%S - Stations-Code\n    %%L - Location-Code\n    %%C - Kanal\n    %%N - Network-Code\n    %%j - Julianischer Tag\n\n  Der Standardwert ist 'out/%%S/%%y-%%m-%%d-%%C.mseed'.\n\n--cut=SECONDS\n\n  Schneidet die Daten in Stücke von SECONDS Sekunden. Der Standardwert ist\n  86400, also ein Tag.\n\n--no-cut\n\n  Schaltet das Schneiden der MiniSEED-Dateien ab und erzeugt stattdessen eine\n  große Datei pro Kanal.\n\n--start-time=TIME\n--end-time=TIME\n\n  Begrenzt die konvertierung auf den angegebenen Zeitraum.\n  Wenn diese Optionen benutzt werden, enthalten die erzeugten MiniSEED-Dateien\n  keine Samples vor der angegebenen Startzeit oder nach der angegebenen Endzeit.\n\n  Die Standardwerte sind der Anfang und das Ende der Aufzeichnung.\n\n  Die Zeiten müssen als ISO-String angegeben werden, das heißt im Format\n  'YYYY-MM-DDTHH:MM:SSZ'.\n\n--logfile=FILE\n\n  Erstellt eine Logdatei unter FILE.\n  Die Logdatei enthält alle wichtigen Ereignisse, die während des Konvertierens\n  auftreten. Das sind hauptsächlich allgemeine Informationen über die Daten,\n  Namen von erstellten Dateien und eventuelle Fehler.\n\n--auxfile=FILE\n\n  Erstellt eine CSV-Datei mit zusätzlichen Daten unter FILE.\n  Die CSV-Datei enthält die Statusdaten des Datenloggers, wie z.B. Temperatur\n  oder Batteriespannung.\n\n--ignore-skew\n\n  Die zweite Synchronisierung wird nicht verwendet. Der Zeitfehler wird\n  hierdurch im Mittel verdoppelt, daher gibt diese Option immer eine Warnung aus\n  und sollte nicht verwendet werden.\n\n--resample\n\n  Die Skew-Korrektur wird durchgeführt, indem die Daten auf die nominelle Samplerate\n  resampled werden.\n\nBeispiele\n---------\n\nKonvertiert die Datei 'ST007.6d6' mit den Standardeinstellungen nach MiniSEED.\nDie Option '--station' wird immer benötigt!\n\n  $ 6d6mseed --station=ST007 ST007.6d6\n\nKonvertiert die Datei 'ST007.6d6' mit einem Network-Code und benutzerdefinierten\nMiniSEED-Dateinamen.\n\n  $ 6d6mseed ST007.6d6 \\\n      --station=ST007 \\\n      --network=XX \\\n      --output=%%N/%%S/%%y-%%m-%%d-%%C.mseed\n\n",
  .usage_6d6read_s = "6d6read\n=======\n\nDas Programm '6d6read' konvertiert Rohdaten vom 6D6-Datenlogger in das\nSend2X-Format.\nDies kann nützlich sein, wenn noch eine große Anzahl von alten Datenloggern\nvorhanden ist und eine einheitliche Datenverarbeitung gewünscht ist.\n\nVerwendung\n----------\n\n  %s [-q|--no-progress] < in.6d6 > out.s2x\n\nDie Eingabedatei ist eine .6d6-Datei und wird mit einer öffnenden spitzen\nKlammer angegeben '<'.\nDie Ausgabedatei ist eine .s2x-Datei und wird mit einer schließenden spitzen\nKlammer angegeben '>'.\n\nWenn Eingabedatei eine gültige .6d6-Datei ist, beginnt der Konvertierungs-\nvorgang und der Fortschritt wird im Terminal angezeigt. Um die Ausgabe z.B. für\nautomatisierte Scripts zu unterdrücken, kann die Option '-q' bzw.\n'--no-progress' verwendet werden.\n\nBeispiele\n---------\n\nKonvertiert die Datei 'station-007.6d6' ins Send2X-Format:\n\n  $ 6d6read < station-007.6d6 > station-007.s2x\n\nKonvertiert die Datei 'x.6d6' nach 'y.s2x' ohne Fortschrittsanzeige:\n\n  $ 6d6read --no-progress < x.6d6 > y.s2x\n",
  .using_channel_mapping = "Benutze folgende Kanalzuordnungen:\n",
  .version_ss = "6D6 Compatibility Tools Version %s (%s)\n\n",
};
const I18n *de_DE = &_de_DE;
const I18n *i18n = &_en_GB;
#endif
