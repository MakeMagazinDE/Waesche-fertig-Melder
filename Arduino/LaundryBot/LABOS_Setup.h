/***************************************************************************
                            LABOS/1, Version 1.0
                            Kilian Hofmann, 2020
                     ----- SETUP-Konfigurationen -----

****************************************************************************/
/*
  Hier sind alle zu personaliserenden Konfigurationseinstellungen zusammengefasst.
  Bei einem Softwareupdate müssen so keine Config-Einstellungen innerhalb der neuen
  Dateien vorgenommen werden.
  Update kopieren, ggf. neue Parameter ergänzen, Sketch hochladen und gut.

*/

/*---------------------------------------------------------------------*/
/* ---------- Diese #defines müssen ggf. modifiziert werden ---------- */
/*---------------------------------------------------------------------*/

// Anmeldedaten des WLANS
#define WIFI_SSID "<WLAN SSID>" //  WLAN-SSID (Name)
#define PASSWORD "<WLAN Pwd>"       //  WLAN-Passwort

// Telegram-Einstellungen

#define MASTER_USER "" // chatID des Watchbot Owners. Muss vor Nutzung gesetzt werden! Siehe Make: Artikel
#define TELEGRAM_BOT_TOKEN "<Http API Bot-Token>" //  persönlicher Bot-Token. Muss vor Nutzung gesetzt werden! Siehe Make: Artikel

  // Laundry-Gerätekonfiguration: Grundeinstellungen
#define WASHER_ENABLED true // Waschmaschine überwachen = true, wenn nicht false
#define DRYER_ENABLED true  // Trockner überwachen
#define ALERT_ENABLED true  // Wassermelder überwachen

  /** SignalPattern des jeweiligen Laundry-Geräts. Variante hinter das #define der Maschine eintragen
    off_flashing, // aus bei laufendem Laundry-Gerät, blinkend wenn fertig
    off_on,       // aus bei laufendem Laundry-Gerät, an wenn fertig
    on_flashing,  // an bei laufendem Laundry-Gerät, blinkend wenn fertig
    on_off        // an bei laufendem Gerät, aus wenn fertig   **/

#define SIGNAL_PATTERN_WASHER off_on     // Signal-LED: aus bei laufender Maschine, blinkend wenn fertig
#define SIGNAL_PATTERN_DRYER on_flashing // Signal-LED: an bei laufendem Trockner, blinkend wenn fertig

  /*-------------------------------------------------------------------------------*/
  /* ---------- Diese Parameter können eventuell angepasst werden  --------------- */
  /*-------------------------------------------------------------------------------*/

#define MAX_USER 8 // Maximale Anzahl von Nutzern

#define WASHER_REMIND_ME true       // Erinnerungsfunktion Waschmaschine
#define DRYER_REMIND_ME true        // Erinnerungsfunktino Trockner
#define LAUNDRY_REMINDER_OFFSET_MINUTES 15 // Zeit bis zur nächsten Erinnerungsmitteilung in Minuten. Wird verdoppelt, bevor es nervt: 15,30,60,120... Minuten

  // Beliebige Gerätenamen, werden so in den Ausgaben genutzt. Horst und Paula geht also auch.

#define WASHER_DEVICE_NAME "Waschmaschine"
#define DRYER_DEVICE_NAME "Trockner"
#define ALERT_DEVICE_NAME "Wassermelder"

#define ADS_LDR_MAX_READ_SECONDS 2 //  Ermittlungszeitraum der LDR Maximalwerte (wichtig bei blinkenden Fertig-LEDs am Gerät)

#define ALERT_REMINDER_OFFSET_MINUTES 2 // 2 Minuten bis zum nächsten Wasseralarm.

  /* Wert, der nach der LDR-Messung hinzugefügt/abgezogen wird,
    um Sensor-Schwankungen auszugleichen.
    Nur ändern bei Problemen, gleicht Schwankungen der LDRs aus. */

#define THRESHOLD_OFFSET 50        // Laundry Maschinen
#define WATER_THRESHOLD_OFFSET 200 // WaterAlert

  /*  Sensoren überhaupt connected? Kann nur durch niedrige Messwerte festgestellt werden.
    Bei kompletter Dunkelheit kann der Wert unterschritten werden - das führt zu Fehlalarmen.
    -1 deaktiviert die Sensorüberwachung. */


#define ADS_NO_SIGNAL_THRESHOLD 3
