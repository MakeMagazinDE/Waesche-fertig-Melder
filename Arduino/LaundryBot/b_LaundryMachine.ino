/***************************************************************************
                            LABOS/1, Version 0.9
                            Kilian Hofmann, Feb. 2020
                     ----- LaundryMachine-Routinen -----

****************************************************************************/

// --------------- Initialisierung des Objekts

void LaundryMachine ::init()
{

  pinMode(PinLedCase, OUTPUT);
  pinMode(PinLedSensor, OUTPUT);

  StateReady = false;
  PinValue = -1;
  SensorOk = true;
  RemindMe = true;

  // Textkonservenverarbeitung

  MsgReady = labstrsprintf(MsgReady, DeviceName);

  MsgNotReady = labstrsprintf(MsgNotReady, DeviceName);

  Serial.println("Initial: Device: " + String(DeviceName) + ", Pin: " + String(PinLDR) + ", Value: " + String(PinValue) + ", Threshold: " + String(Threshold));
  return;
}

// --------------- Ready Status. Wird beispielsweise in der Loop() abgefragt.

bool LaundryMachine ::Ready()
{

  if (!enabled) // Tue nix wenn Laundry-Gerät nicht enabled
    return false;

  Serial.println("ReadyCheck: Device: " + String(DeviceName) + ", Pin: " + String(PinLDR) + ", Value: " + String(PinValue) + ", Threshold: " + String(Threshold));

  if (PinValue <= ADS_NO_SIGNAL_THRESHOLD and ADS_NO_SIGNAL_THRESHOLD > -1 and SensorOk) // Einmalige Fehlermeldung und Sensorstatus wird gesetzt. Bis zum Neustart werden alle Messungen ignoriert.
  {
    Msg.SendStatus(DeviceName + ERR_NO_VALUES + String(PinValue));
    Serial.println("Sensor Error: Device: " + String(DeviceName) + ", Pin: " + String(PinLDR) + ", Value: " + String(PinValue) + ", Threshold: " + String(Threshold));
    SensorOk = false;
    PinValue = -1;
    return false;
  }

  if (LEDSignalPattern == off_flashing or LEDSignalPattern == off_on) // LED-Signalpattern dunkel --> hell: PinValue muss größer Threshold sein
  {
    if (PinValue >= Threshold)
      return true;
    else
      return false;
  }
  else
  {
    if (PinValue <= Threshold and SensorLEDMachineOn)
      return true;
    else
      return false; // LED-Signalpattern hell --> dunkel und Maschine läuft: PinValue muss kleiner als Threshold sein
  }
}

// --------------- Eigentliche Messung durchführen und "Läuft die Maschine" im Fall der off_- Signalpattern

bool LaundryMachine ::Measure()
{

  int iRead;

  if (!SensorOk or !enabled) // Tue nix im Fehlerfall oder wenn Laundry-Gerät nicht enabled
    return false;

  iRead = ads.readADC_SingleEnded(PinLDR); // Pin auslesen

  if (LEDSignalPattern == off_flashing or LEDSignalPattern == off_on) // LED Logik Von LED dunkel auf hell
  {
    if (iRead > PinValue)
      PinValue = iRead; // PinValue mit dem höchstgelesenen Wert belegen
  }

  else // LED Logik  von hell auf dunkel

  {
    if (PinValue == -1)
      PinValue = iRead; // Setze den Wert im Initialfall nach Reset. Der Ready-Fall wird bei on_-Logik mit kleiner Threshold abgefragt. -1 ist immer kleiner Threshold.

    if (iRead >= Threshold - 100 and !SensorLEDMachineOn) // On-Fall: Maschine wurde erstmals gestartet, weil der Threshold ÜBERSCHRITTEN wurde. Zusätzlich mit -100 Puffer.
    {
      SensorLEDMachineOn = true;
      LightsRunning();
      PinValue = iRead;
    }

    if (iRead < PinValue)
      PinValue = iRead; // Standardlogik. Der kleinere Wert wird übernommen
  }

  return true;
}

// ---------------  Liefert den aktuellen Wert am Pin und kümmert sich ums Fehlerhandling im Fall einer LDR Threshold Messung

int LaundryMachine ::GetPinValue()
{

  int iMax = -1; // Maximalwert am Pin
  int iRead = 0; // Gelesener Wert am Pin

  if (!SensorOk or !enabled) // Tue nix im Fehlerfall oder wenn Laundry-Gerät nicht enabled
    return -1;

  for (int i = 0; i <= ADS_LDR_MAX_READ_SECONDS * 20; i++)
  { // Messyzklus....

    iRead = ads.readADC_SingleEnded(PinLDR);

    if (LEDSignalPattern == off_flashing or LEDSignalPattern == off_on)
    { // höherer oder niedrigerer gemessener Wert, je nach Sensorlogik
      if (iRead > iMax)
        iMax = iRead;
    }
    else
    {
      if (iMax == -1)
        iMax = iRead; // Initialwert bei hell zu dunkel. Es soll ja der "dunkelste" Hellwert gefunden werden.
      if (iRead < iMax)
        iMax = iRead;
    }

    delay(50);
  }

  // Sensor liefert verdächtig niedrige Werte? Eventuell liegt ein Verbindungsproblem vor
  // Einmalige Fehlermeldung und Sensorstatus wird gesetzt.
  // Bis zum Neustart werden alle Messungen ignoriert. Bei Threshold von -1 ist die Überwachung inaktiv.

  if (iMax <= ADS_NO_SIGNAL_THRESHOLD and ADS_NO_SIGNAL_THRESHOLD > -1 and SensorOk)
  {
    Msg.SendStatus(DeviceName + ERR_NO_VALUES + String(iMax));
    SensorOk = false;
    iMax = -1;
  }

  return iMax;
}

// --------------- Setzen des eigentlichen Ready Status der Maschine. Ausgabe der Meldung, Setzen der LEDs und des internen Status und Reminderhandling

void LaundryMachine ::SetReadyStatus()
{

  char text[250];
  char buf[100];

  if (!SensorOk or !enabled) // Tue nix im Fehlerfall oder wenn Laundry-Gerät nicht enabled
    return;

  if (!StateReady) // Wenn der Status erstmalig auf true gesetzt wurde, schicke Message an alle
  {
    StateReady = true;
    Serial.println("SetReady: Device: " + String(DeviceName) + ", Pin: " + String(PinLDR) + ", Value: " + String(PinValue) + ", Threshold: " + String(Threshold));

    RepeatMsgStatus();

    ReadyReminded = 0;            // Wiederholungsmultiplikator
    TimeReady = millis();         // Startzeit                             
    TimeReadyReminder = millis() + (ReadyTimerOffset * 60000); // Erinnerung nach 15,30,60,120... Minuten
  }
  else // Ansonsten sind wir im Wiederholungsfall. Zeit für einen Reminder?
  {
    if (millis() >= TimeReadyReminder and RemindMe)
    {

      Serial.println("SetReady Reminder: Device: " + String(DeviceName) + ", Pin: " + String(PinLDR) + ", Value: " + String(PinValue) + ", Threshold: " + String(Threshold));
      if (AlertDevice)
      {
        TimeReadyReminder = millis() + (ReadyTimerOffset * 60000); // Regekmäßig nerven. Potenz egal.
        RepeatMsgStatus();
      }
      else
      {
        DeviceName.toCharArray(buf, DeviceName.length() + 1);
        sprintf(text, MSG_READY_REMINDER, buf, int(ReadyTimerOffset * pow(2, ReadyReminded)));
        Msg.SendStatus(String(text));
        ReadyReminded++;
        TimeReadyReminder = TimeReady + (ReadyTimerOffset * pow(2, ReadyReminded) * 60000);
        
      }
    }
  }
}
 

// --------------- Setzen des Running Status der Maschine. Ausgabe der Meldung, Setzen der LEDs und des intenen Status

void LaundryMachine ::SetRunningStatus()
{

  if (!SensorOk or !enabled) // Tue nix im Fehlerfall oder wenn Laundry-Gerät nicht enabled
    return;

  PinValue = -1;

  if (StateReady)
  {
    Serial.println("SetRunning: Device: " + String(DeviceName) + ", Pin: " + String(PinLDR) + ", Value: " + String(PinValue) + ", Threshold: " + String(Threshold));
    StateReady = false;
    SensorLEDMachineOn = false; // On Sensor LED Modell: Wert wird zurückgesetzt - System geht davon aus, daß die Maschine aus ist.
    TimeReadyReminder = 0;
    SensorLEDMachineOn = false; // On Sensor LED Modell: Wert wird zurückgesetzt - System geht davon aus, daß die Maschine aus ist.
    RepeatMsgStatus();
  }
}

void LaundryMachine ::SetThreshold()
{

  if (LEDSignalPattern == off_flashing or LEDSignalPattern == off_on)
    Threshold = NewThreshold + THRESHOLD_OFFSET;
  else
    Threshold = NewThreshold - THRESHOLD_OFFSET;

  PinValue = -1;
}

//  --------------- Wiedergabe des aktuellen den Maschinenstatus...

void LaundryMachine ::RepeatMsgStatus(bool forall)
{

  if (!enabled)
    return; // Wenn Laundry-Gerät nicht enabled, tue nix.

  if (!SensorOk)
  { // Im Fehlerfall nochmal die Meldung ausgeben.
    if (forall)
      Msg.SendStatus(DeviceName + ERR_NO_VALUES);
    else
      Msg.Send(DeviceName + ERR_NO_VALUES);

    return;
  }

  if (StateReady) // Maschine fertig....
  {
    LightsReady();

    if (AlertDevice) // Im Fall von Wasser bekommen alle User eine Meldung
      Msg.SendAlert(MsgReady);
    else
    {
      if (forall)
        Msg.SendStatus(MsgReady);
      else
        Msg.Send(MsgReady);
    }
  }
  else // oder nicht fertig.
  {
    LightsRunning();
    if (forall)
      Msg.SendStatus(MsgNotReady);
    else
      Msg.Send(MsgNotReady);
  }
}

//  --------------- LEDs an

void LaundryMachine ::LightsOn()
{

  digitalWrite(PinLedCase, HIGH);
  digitalWrite(PinLedSensor, HIGH);
}

//  --------------- LEDs aus

void LaundryMachine ::LightsOff()
{

  digitalWrite(PinLedCase, LOW);
  digitalWrite(PinLedSensor, LOW);
}

//  --------------- LEDs Maschine läuft

void LaundryMachine ::LightsRunning()
{

  digitalWrite(PinLedCase, LOW);
  if (LEDSignalPattern == on_off or LEDSignalPattern == on_flashing and SensorLEDMachineOn)
    digitalWrite(PinLedSensor, HIGH);
  else
    digitalWrite(PinLedSensor, LOW);
}

//  --------------- LEDs Fertig

void LaundryMachine ::LightsReady()
{

  digitalWrite(PinLedCase, HIGH);
  if (LEDSignalPattern == on_off or LEDSignalPattern == on_flashing)
    digitalWrite(PinLedSensor, LOW);
  else
    digitalWrite(PinLedSensor, HIGH);
}

// --------------- Kleine Lightshow, beispielsweise für den Fall von fehlerhaften Sensoren.

void LaundryMachine ::FlashLights()
{

  if (!enabled)
    return; // Wenn Laundry-Gerät nicht enabled, tue nix.

  for (int i = 0; i < 5; i++)
  {
    LightsOn();
    delay(100);
    LightsOff();
    delay(100);
  }
}

// --------------- Lässt die Sensoren LEDs blinken, falls erforderlich

void LaundryMachine ::SensorFlash() // Blink Blink für Sensoren
{
  if (!SensorOk or !enabled) // Tue nix im Fehlerfall oder wenn Laundry-Gerät nicht enabled
    return;

  if (StateReady)
  {
    if (LEDSignalPattern == on_flashing or LEDSignalPattern == off_flashing)
    {
      if (TimeSensorFlash <= millis())
      {
        SensorLEDon = !SensorLEDon;

        if (SensorLEDon)
          digitalWrite(PinLedSensor, HIGH);
        else
          digitalWrite(PinLedSensor, LOW);

        TimeSensorFlash = millis() + 500;
      }
    }
  }
}

/************************************************

  Klassenunabhängige Laundry Routinen

************************************************/

// ---------------  Simples lesen der Thresholds.

void ReadThresholds()
{

  String fline;

  if (SPIFFS.exists("Configuration.txt"))
  { // Existiert eine Datei?

    Serial.println("Config file not found");

    //  Lesen der Daten in entsprechende Variablen
    File f = SPIFFS.open("Configuration.txt", "r");
    if (!f)
    { //  Datei ist vorhanden, kann aber nicht gelesen werden? Hier ist was faul mit dem ESP
      Msg.Send(ERR_CONFIG_NOT_READ);
    }

    fline = f.readStringUntil('\n');
    Washer.Threshold = fline.toInt();
    fline = f.readStringUntil('\n');
    Dryer.Threshold = fline.toInt();
    fline = f.readStringUntil('\n');
    WaterAlert.Threshold = fline.toInt();

    f.close();

    Serial.println("Config Read: Wascher: " + String(Washer.Threshold) + ", Dryer: " + String(Dryer.Threshold) + ", Alert: " + String(WaterAlert.Threshold));
  }
  else //speichern der Standardwerte, wenn Datei nicht existiert --> Dateineuanlage
  {
    Serial.println("Config file creation");
    SaveNewThresholds(); // Speichern der aktuellen Thresholds = Anlage einer neuen Datei
  }
}

// ---------------  Simples speichern der Thresholds.

bool SaveNewThresholds()
{

  // Config Datei öffnen
  File a = SPIFFS.open("Configuration.txt", "w");

  if (!a)
  {
    Msg.Send(ERR_CONFIG_NOT_SAVED);
    return false;
  }

  // Zu speichernde Werte werden neu gesetzt

  a.println(String(Washer.Threshold));
  a.println(String(Dryer.Threshold));
  a.println(String(WaterAlert.Threshold));

  a.close();

  return true;
}
