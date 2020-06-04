/***************************************************************************
                            LABOS/1, Version 0.9
                            Kilian Hofmann, Feb. 2020
                    ----- Messages: Telegram-Variante -----

****************************************************************************/

// ---------------  Initialisierung, Lesen der gespeicherten User

void TelegramMessaging ::init()
{

  iMsgQueue = 0; // Anzahl der Messages, die in der Queue warten. Diese werden zuerst abgearbeitet, bis die nächsten gelesen werden
  iMsgId = -1;   // I
  lastTimeChecked = millis();

  bot.longPoll = 1; //  Wartezeit des Bots auf eine neue Nachricht, in Sekunden. Dann gehts weiter

  // Defaultuser == Masteruser in Liste eintragen

  //Init user array
  for (int i = 0; i < MAX_USER; i++)
  {
    Users[i].id = "";
    Users[i].cname = "";
    Users[i].subscribed = false;
  }

  DefinedUser = 1;
  Users[0].id = MASTER_USER;
  Users[0].subscribed = true;

  // Keyboards

  CustomKeyboard = sNormalKeyboard;

  // User laden
  ReadUsers();
}

// ---------------  Sind noch ungelesene Messages in der Queue? Wenn ja, Anzahl merken in iMsgQueue

bool TelegramMessaging ::QueueFilled()
{

  iMsgId = -1;
  if (millis() > lastTimeChecked + TELEGRAM_READ_DELAY)
  {
    if (iMsgQueue = bot.getUpdates(bot.last_message_received + 1) > 0)
      return true;

    lastTimeChecked = millis();
  }
  return false;
}

/* ---------------  Nächste Nachricht aus der Queue lesen. Wenn der User bekannt ist, Text extrahieren und aktuellen User setzen.
                    Unbekannte User/Messages ignorieren, die Nächste aus der Queue auslesen                                       */

bool TelegramMessaging ::GetNextMessage()
{

  sMsgReceived = "";
  iMsgId++;

  while (iMsgId < iMsgQueue) // Noch was drin? dann lesen
  {

    // User berechtigt und überhaupt ein Befehl?

    if (UserAuthorized(String(bot.messages[iMsgId].chat_id), bot.messages[iMsgId].from_name)) // and bot.messages[iMsgId].text.indexOf( "/" ) != -1 )
    {
      sMsgReceived = bot.messages[iMsgId].text;       // Auslesen und zuweisen der Nachrcht
      chat_id = String(bot.messages[iMsgId].chat_id); // Aktuellen User setzen für Interaktion
      firstname = bot.messages[iMsgId].from_name;
      Serial.println("Get Message: iMsgQueue " + String(iMsgQueue) + ", iMsgId " + String(iMsgId) + ", " + sMsgReceived);
      return true;
    }
    else
    {
      iMsgId++;
    }
  }

  return false;
}

// ---------------  Usercheck - ist der User berechtigt, den Bot zu nutzen?

bool TelegramMessaging ::UserAuthorized(String UserId, String Username)
{

  int i;
  bool found = false;

  // Prüfe alle bekannten User
  for (int i = 0; i < DefinedUser and not found; i++)
  {
    if (Users[i].id == UserId)
    {

      if (Users[i].cname.isEmpty() or Users[i].cname == "???") // Username noch leer? Dann füllen und speichern
      {
        Users[i].cname = Username;
        WriteUsers();
      }

      found = true;
      break;
    }
  }

  if (!found)
  { // Wenn nichts gefunden wurde...

    if (UserId != MASTER_USER)
    {

      bot.sendMessage(UserId, MSG_USER_NOT_ALLOWED + UserId + ", " + Username, "Markdown");

      // Ausgabe des unauthorisierten Users an alle Nutzer.
      SendStatus(MSG_UNAUTHORIZED_USER + UserId + ", " + Username);

      Serial.println("Unauthorized user access: " + UserId + ", " + Username);
    }

    else

    {
      // Errorhandling, Fehler im Userhandling - Notzugang für den Master

      Serial.println("Master Access due to error");
      Users[0].id = MASTER_USER;
      Users[0].cname = "Master";
      Users[0].subscribed = true;
      SendStatus(ERR_USER_HANDLING);
      found = true;
    }
  }
  return found;
}

// ---------------  Wurde dieser Befehl(cmd) eingegeben? Wenn ja, einer mit Parameter?

bool TelegramMessaging ::IsCommand(String cmd)
{

  cmd.trim();

  if (cmd.indexOf("/") != 0)
    return false; // Kein Slash auf erster Position, kein Befehl

  if (sMsgReceived.indexOf(" ") != -1)
  { // Befehl mit Parameter? Dann erst aufsplitten vor dem Vergleich

    cmdParam = sMsgReceived.substring(sMsgReceived.indexOf(" ") + 1);
    cmdParam.trim();

    if (cmdParam.isEmpty())
      return false;

    sMsgReceived = sMsgReceived.substring(0, sMsgReceived.indexOf(" "));

    Serial.println("cmd: " + sMsgReceived + ", extracted: " + cmdParam);
  }

  if (sMsgReceived.equalsIgnoreCase(cmd)) // Eigentlicher Vergleich, unabhängig der Groß-/Kleinschreibung
    return true;
  else
    return false;
}

// ---------------  Sende eine Nachricht an den aktuellen User (chat_id)

bool TelegramMessaging ::Send(String Message)
{

  bool sendstatus = false;

  //  Senden einer Nachricht ohne dass eine andere Tastatur angezeigt wird

  sendstatus = SendMessage(chat_id, Message);

  return sendstatus;
}

// ---------------  Sende eine Status-Nachricht an alle User mit subscription

bool TelegramMessaging ::SendStatus(String Message)
{

  int i = 0;

  for (i = 0; i < DefinedUser; i++)
  {

    Serial.println("SendStatus " + Message + " to " + Users[i].id + ", i: " + String(i) + " subscribed: " + String(Users[i].subscribed));

    if (Users[i].subscribed)
      SendMessage(Users[i].id, Message);
  }

  return true;
}

// ---------------  Sende die (Wasser-) Alarm Meldung 5 mal an alle User, im Abstand von einer Sekunde

bool TelegramMessaging ::SendAlert(String Message)
{

  for (int r = 0; r < 5; r++)
  {
    for (int i = 0; i < DefinedUser; i++)
      SendMessage(Users[i].id, Message);
    delay(1000);
  }

  return true;
}

void TelegramMessaging ::SendNoAuth()
{

  String showtext = labstrsprintf(MSG_USR_AUTH, firstname);
  Msg.Send(showtext);
}

// ---------------  Eigentliche Nachrichtensendung, mit der Berücksichtigung des Befehls-Keyboards (Tastatur)

bool TelegramMessaging ::SendMessage(String User, String Message)
{

  bool sendstatus;

  if (CustomKeyboard.equals(""))
    sendstatus = bot.sendMessage(User, Message, "Markdown");
  else
    sendstatus = bot.sendMessageWithReplyKeyboard(User, Message, "", CustomKeyboard, true);

  Serial.println("Msg " + Message + " to " + User + ", ReturnCode: " + String(sendstatus));

  return sendstatus;
}

// ---------------  Entscheidungsnachricht an den aktuellen User: Es wird das Ja/Nein Keyboard eingeblendet, das vorherige gepuffert.
//                  Anschließend wird das Keyboard wieder zurückgesetzt.

bool TelegramMessaging ::Decision(String Command, String Question)
{

  String keys = CustomKeyboard;

  CurrentDecision = Command;

  CustomKeyboard = sDecisionKeyboard;
  Send(Question);

  CustomKeyboard = keys;

  return true;
}

// ---------------  Nachricht mit Tastaturwechsel

bool TelegramMessaging ::SwitchKeys(String Message, String newkeyboard)
{

  CustomKeyboard = newkeyboard;
  CurrentDecision = "";
  if (Message != "")
    Send(Message);

  return true;
}

// ---------------  Neuen User hinzufügen und gleich in der Datei abspeichern

bool TelegramMessaging ::AddUser(String newUser)
{

  bool ok = false;
  int i;

  if (newUser == MASTER_USER)
  {
    Send(ERR_USR_MASTER);
    return false; // Der neue User kann nicht der Masteruser sein.
  }
  // User schon in Liste?
  for (int i = 0; i < DefinedUser; i++)
  {
    if (Users[i].id.equalsIgnoreCase(newUser))
    {
      Send(ERR_USR_IN_LIST);
      return false;
    }
  }

  if (DefinedUser < MAX_USER) //  and newUser != MASTER_USER)  // Noch Platz in der Liste?
  {
    Users[DefinedUser].id = newUser;
    Users[DefinedUser].cname = "???"; // Chatname ist solange unbekannt, bis sich der User das erste Mal beim Bot anmeldet
    Users[DefinedUser].subscribed = true;
    DefinedUser++;

    Serial.println("Write User: DefU:" + String(DefinedUser) + ",  " + Users[DefinedUser - 1].id + ", " + Users[DefinedUser - 1].cname + " Subscribed: " + String(Users[DefinedUser - 1].subscribed));

    ok = WriteUsers();

    if (ok) // Wenn alles gut - Testnachricht mit Prüfung auf Erfolg
    {
      ok = SendMessage(newUser, MSG_USR_ADD_HELLO);
      if (!ok)
        Send(MSG_USR_HELLO_ERROR); // Nachricht kam nicht an. Wahrscheinlich falsche ID.
    }
    else
      Send(ERR_USR_FILE);

    if (!ok) // nix gut
    {

      DefinedUser--;
      Users[DefinedUser].id = "";
    }
  }
  else
    Send(ERR_USR_MAX_NO);

  return ok;
}

// ---------------  User aus der Liste entfernen falls bekannt, Userdatei sichern

bool TelegramMessaging ::RemoveUser(String delUser)
{

  bool ok = false;

  if (delUser == MASTER_USER or DefinedUser == 1)
  {
    Send(ERR_USR_DEL_MASTER);
    return false;
  }

  for (int i = 0; i < MAX_USER - 1; i++) // Check die gesamte Liste...
  {
    if (Users[i].id == delUser or ok)
    { // User gefunden?

      ok = true;

      Users[i].id = Users[i + 1].id; // Schiebe die nachfolgenden User einen Platz vor
      Users[i].cname = Users[i + 1].cname;
      Users[i].subscribed = Users[i + 1].subscribed;
    }
  }

  if (ok) // Wurde der User gefunden? Dann Datei schreiben
  {
    Users[MAX_USER - 1].id = "";
    Users[MAX_USER - 1].cname = "";
    DefinedUser--;
    ok = WriteUsers();
    if (ok)
      SendMessage(delUser, MSG_USR_GOODBYE);
  }

  return ok;
}

// ---------------  Subscribed Status (true/false) für den aktuellen User setzen und Datei schreiben

bool TelegramMessaging ::SaveSubscribeStatus(bool subscribed)
{

  bool ok = false;
  for (int i = 0; i < MAX_USER; i++)
  {
    if (Users[i].id == chat_id)
    {
      Users[i].subscribed = subscribed;
      ok = true;
      break;
    }
  }

  if (ok)
    ok = WriteUsers();

  return ok;
}

// ---------------  Liste der aktuellen User ausgeben

String TelegramMessaging ::PrintUserList()
{

  String text = "--------------- \n";
  String sub;

  for (int i = 0; i < DefinedUser; i++)
  {

    // Subcribe Command als Status verwenden....
    if (Users[i].subscribed)
      sub = CMD_USR_SUBSCRIBE;
    else
      sub = CMD_USR_UNSUBSCRIBE;

    Serial.println("sub before " + sub);

    sub.replace("/", "");

    Serial.println("sub after " + sub);

    text = text + "User " + String(i + 1) + ": " + Users[i].id + ", " + String(Users[i].cname) + ", ";
    text = text + sub; // Seltsam, aber in einem Schritt hat das nicht funktioniert
    text = text + " \r\n";
  }

  text = text + " \n --------------- \n";

  return text;
}

// -------------------------- Master User? ------------------------------

bool TelegramMessaging ::UserIsMaster()
{
  if (chat_id == MASTER_USER)
    return true;
  else
    false;
}

// ---------------  User-Datei wird ausgelesen, das User Array gefüllt.

bool TelegramMessaging ::ReadUsers()
{

  String sLine;
  bool ok;

  File f = SPIFFS.open("user.txt", "r");
  if (!f)
  {
    Msg.Send(ERR_USR_NOT_READ);
    return false;
  }

  DefinedUser = 0; // Maximalzähler der User

  for (int i = 0; i < MAX_USER; i++)
  {
    sLine = f.readStringUntil('\n');
    if (sLine.isEmpty())
      break;
    Users[i].id = sLine.substring(0, sLine.indexOf(";"));
    sLine = sLine.substring(sLine.indexOf(";") + 1);
    Users[i].cname = sLine.substring(0, sLine.indexOf(";"));

    Serial.println("Read User: " + sLine);

    sLine = sLine.substring(sLine.indexOf(";") + 1);

    sLine.trim();

    if (sLine == "S")
      Users[i].subscribed = true;
    else
      Users[i].subscribed = false;

    Serial.println("Read User: " + Users[i].id + ", " + Users[i].cname + " Subscribed: " + String(Users[i].subscribed));

    DefinedUser++;
  }
  f.close();
  return true;
}

// ---------------  User-Datei wird geschrieben

bool TelegramMessaging ::WriteUsers()
{

  // Config Datei öffnen
  File a = SPIFFS.open("user.txt", "w");
  String sub;

  if (!a)
  {
    Msg.Send(ERR_USR_NOT_SAVED);
    Serial.println("Write User: Fehler bei Dateianlage");
    return false;
  }
  for (int i = 0; i < DefinedUser; i++)
  {

    if (Users[i].subscribed)
      sub = "S";
    else
      sub = "U";
    a.println(Users[i].id + ";" + Users[i].cname + ";" + sub);

    Serial.println("Write User: " + Users[i].id + "," + Users[i].cname + ", Subscribed: " + String(Users[i].subscribed));
  }
  a.close();
  return true;
}
