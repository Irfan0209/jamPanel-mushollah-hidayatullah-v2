//const char msg[] PROGMEM = "MUSHOLLAH HIDAYATULLAH RT19/RW03,DODOKAN,TANJUNGSARI";
 char *  pasar[]  = {"WAGE", "KLIWON", "LEGI", "PAHING", "PON"}; 
 char *  Hari[]  = {"AHAD","SENIN","SELASA","RABU","KAMIS","JUM'AT","SABTU"};
//const char * const bulanMasehi[] PROGMEM = {"JANUARI", "FEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOVEMBER", "DESEMBER" };
//const char msg1[] PROGMEM ="LURUSKAN DAN RAPATKAN SHAFF SHOLAT";
 char * namaBulanHijriah[] = {
    "MUHARRAM", "SHAFAR", "RABIUL AWAL",
    "RABIUL AKHIR", "JUMADIL AWAL", 
    "JUMADIL AKHIR", "RAJAB",
    "SYA'BAN", "RAMADHAN", "SYAWAL",
    "DZULQA'DAH", "DZULHIJAH"
};
// const char jadwal[][8] PROGMEM = {"SUBUH", "TERBIT", "DZUHUR", "ASHAR", "MAGRIB", "ISYA'"};


void drawTime()
{
    if (adzan) return;
    
    RtcDateTime now = Rtc.GetDateTime();
    uint8_t detik = now.Second();
    uint8_t menit = now.Minute();
    uint8_t jam = now.Hour();
    char buff_jam[3];
    char buff_menit[3];
//    static bool flag1 = false,flag2 = false;
    
    snprintf(buff_jam, sizeof(buff_jam), "%02d", jam);
    snprintf(buff_menit, sizeof(buff_menit), "%02d", menit);

    fType(4);
    Disp.drawText(16, -1, buff_jam); 
    Disp.drawText(34, -1, buff_menit); 

    // Blink colon (titik dua) setiap detik
    if (detik & 1)
    {
        Disp.drawCircle(31, 4, 1);   // Titik atas
        Disp.drawCircle(31, 11, 1);  // Titik bawah
    }
    else
    {
        Disp.drawCircle(31, 4, 1, 0);
        Disp.drawCircle(31, 11, 1, 0);
    }

      logo1(48);
      logo2(0);

    // Garis pemisah bawah
    Disp.drawLine(0, 15, 64, 15);

//     if (jam >= 21  or jam < 3){
//        Disp.setBrightness(20);  // Kecerahan 100% pada jam 02:00 - 10:00
////        flag1 = true;
////        flag2 = false;
//        //Serial.println(F("malam"));
//    } 
//    else if(jam >= 3  or jam < 21 ){
//        Disp.setBrightness(config.brightness);   // Kecerahan 50% pada jam 22:00 - 02:00
////        flag1 = false;
////        flag2 = true;
//        //Serial.println(F("pagi"));
//    }
    //DoSwap = true;
}

void drawDate(){
  static uint16_t x;
  if (reset_x !=0) { x=0;reset_x = 0;}
  static uint16_t fullScroll = 0;
  if(adzan) return;
 
  RtcDateTime now = Rtc.GetDateTime();
  static uint32_t   lsRn;
  uint32_t          Tmr = millis();
   
  uint8_t Speed = config.speedDate;
  uint8_t daynow   = now.DayOfWeek();    // load day Number
  
  char buff_date[100]; // Pastikan ukuran buffer cukup besar
    snprintf(buff_date, sizeof(buff_date), "%s %s %02d-%02d-%04d %02d %s %04dH",
    Hari[daynow], pasar[jumlahhari() % 5], now.Day(), now.Month(), now.Year(),
    Hijir.getHijriyahDate, namaBulanHijriah[Hijir.getHijriyahMonth - 1], Hijir.getHijriyahYear);

  fType(0); 
  if (fullScroll == 0) { // Hitung hanya sekali
  fullScroll = Disp.textWidth(buff_date) + Disp.width();
  }

 if (Tmr - lsRn > Speed) { 
  lsRn = Tmr;
  
  Disp.drawText(Disp.width() - (x++), 25, buff_date);
  DoSwap = true;
  
  if (x >= fullScroll) {
    x = 0;
    show = ANIM_TEXT;
  }
 }
}

void drawSmartText(const char* msg, uint8_t speed,uint8_t fontt) {
  if(adzan) return;

  if (msg == NULL || msg[0] == '\0') {
    nextShowStateRun(); // Langsung baca state selanjutnya
    return;             // Keluar dari fungsi agar tidak membuang CPU
  }
  
  static uint16_t x = 0;
  static uint16_t textW = 0;
  static uint16_t fullScroll = 0;
  static bool isScrolling = false;
  static uint32_t diamTimer = 0; // Timer khusus untuk teks diam
  
  // Reset trigger dari luar (saat ganti menu/tampilan)
  if (reset_x != 0) { 
    x = 0; 
    textW = 0;
    reset_x = 0; 
    return;
  }

  uint32_t Tmr = millis();
  static uint32_t lss = 0;

  // Render frame setiap 45ms (Kecepatan standar agar smooth)
  if ((Tmr - lss) > speed) {
    lss = Tmr;

    fType(fontt);

    // ==============================================================
    // 1. OPTIMASI: Hitung lebar teks SEKALI SAJA di awal
    // ==============================================================
    if (textW == 0) {
      textW = Disp.textWidth(msg);
      
      // Cek apakah lebar teks melebihi lebar layar 6 Panel (DWidth)
      if (textW > DWidth) {
        isScrolling = true;
        fullScroll = textW + DWidth;
      } else {
        isScrolling = false;
        diamTimer = Tmr; // Mulai argon timer untuk durasi teks diam
      }
      
    }

    // ==============================================================
    // 2. LOGIKA TAMPILAN (Diam vs Berjalan)
    // ==============================================================
    if (isScrolling) {
      // --- MODE BERJALAN (Lebih dari 6 Panel) ---
      if (x < fullScroll) {
        ++x;
      } else {
        x = 0; 
        textW = 0;
        fullScroll = 0;
        
        // Panggil fungsi ganti tampilan di sini jika diperlukan
        nextShowStateRun(); 
        return;
      }
      
      // Render teks berjalan, posisi Y = 4 (tengah vertikal)
      Disp.drawText(DWidth - x, 25, msg);
      
    } else {
      // --- MODE DIAM (Kurang dari atau sama dengan 6 Panel) ---
      dwCtr(0, 25, msg);
      
      // PENTING: Karena teks diam tidak punya titik akhir scroll,
      // kita harus memberi batasan waktu tampil (misal: 5000 ms / 5 detik)
      if (Tmr - diamTimer > 5000) {
        textW = 0;
        
        // Panggil fungsi ganti tampilan di sini jika diperlukan
        nextShowStateRun(); 
        return;
      }
    }

    DoSwap = true;
  }
}

void nextShowStateRun()
{ 
  switch(show){
    case ANIM_JAM:  show = ANIM_NAME; break;
    case ANIM_NAME: show = ANIM_TEXT; break;
    case ANIM_TEXT: show = ANIM_JAM;  break;
  };
 
}

/*void runningTextInfo1() {
  static uint32_t x = 0;
  static uint32_t lsRn;
  uint32_t Tmr = millis();
  uint8_t Speed = config.speedText1;

  // Simpan teks di Flash (PROGMEM)
  
  char msg_buffer[50]; // Pastikan cukup besar untuk teks
  strcpy_P(msg_buffer, msg); // Ambil teks dari Flash

  // Hitung panjang teks hanya sekali
  static int16_t fullScroll = 0;
  fType(0);
  
  if (fullScroll == 0) { 
    fullScroll = Disp.textWidth(msg_buffer) + Disp.width();
  }

  // Jalankan animasi scrolling berdasarkan millis()
  if ((Tmr - lsRn) > Speed) { 
    lsRn = Tmr;
    
    
    int16_t posX = Disp.width() - x;
    if (posX < -Disp.textWidth(msg_buffer)) { // Cegah teks keluar layar
      x = 0;
      //show = ANIM_INFO;
      return;
    }

    Disp.drawText(posX, 25, msg_buffer);
    x++; // Geser teks ke kiri
    DoSwap = true;
  }
}

void runningTextInfo2() {
  static uint32_t x = 0;
  static uint32_t lsRn;
  uint32_t Tmr = millis();
  uint8_t Speed = config.speedText2;

  // Simpan teks di Flash (PROGMEM)
  
  char msg_buffer[50]; // Pastikan cukup besar untuk teks
  strcpy_P(msg_buffer, msg1); // Ambil teks dari Flash

  // Hitung panjang teks hanya sekali
  static uint16_t fullScroll = 0;
  fType(0);
  if (fullScroll == 0) { 
    fullScroll = Disp.textWidth(msg_buffer) + Disp.width();
  }

  // Jalankan animasi scrolling berdasarkan millis()
  if ((Tmr - lsRn) > Speed) { 
    lsRn = Tmr;
    //fType(0);
    
    int16_t posX = Disp.width() - x;
    if (posX < -Disp.textWidth(msg_buffer)) { // Cegah teks keluar layar
      x = 0;
      show = ANIM_JAM;
      return;
    }

    Disp.drawText(posX, 25, msg_buffer);
    x++; // Geser teks ke kiri
    DoSwap = true;
  }
}*/

void jadwalSholat(){
 
  if (adzan) return;
  float sholatT[]={JWS.floatSubuh,JWS.floatTerbit,JWS.floatDzuhur,JWS.floatAshar,JWS.floatMaghrib,JWS.floatIsya};

  RtcDateTime now = Rtc.GetDateTime();
  static uint8_t x = 0;
  static bool s = 0; // 0 = masuk, 1 = keluar
  static uint32_t   lsRn;
  uint32_t          Tmr = millis(); 
  static uint8_t   list = 0;

  //int hours, minutes;
  char buff_jam[6]; // Format HH:MM hanya butuh 6 karakter
  char* sholat[] = {"SUBUH", "TERBIT", "DZUHUR", "ASHAR", "MAGRIB", "ISYA'"};   // Buffer untuk menyimpan nama sholat dari PROGMEM

  // Ambil nama sholat dari Flash
  //strcpy_P(sholat, jadwal[list]);
           
    if((Tmr-lsRn)>45) 
      { 
        if(s==0 and x<33){lsRn=Tmr; x++; }
        if(s==1 and x>0){lsRn=Tmr;x--; }
      }

   if((Tmr-lsRn)>1500 and x ==33) {s=1;}

   if (x == 0 && s == 1) { 
    s = 0;
    list = (list + 1) % 6;
  }

  // Ambil nama sholat dari Flash
  //strcpy_P(sholat, jadwal[list]);

  // ===== SHOLAT =====
  float st = sholatT[list];
  uint8_t hh = (uint8_t)st;
  uint8_t mm = (uint8_t)((st - hh) * 60);

  // Format HH:MM
  snprintf(buff_jam, sizeof(buff_jam), "%02d:%02d", hh, mm);

  // Tampilkan teks dengan animasi
  fType(3);
  Disp.drawText(-33 + x, 17, sholat[list]);
  //Disp.drawRect(-33 + x + 29, 17, -33 + x + 29, 23, 0);

  fType(0);
  Disp.drawText(67 - x, 17, buff_jam);
  //Disp.drawRect(67 - x - 1, 17, 67 - x - 1, 23, 0);
  //DoSwap = true;
}

  
/*=============================================================================================*/

void drawAzzan()
{
    static const char *jadwal[] = {"SUBUH", "TERBIT", "DZUHUR", "ASHAR", "TRBNM", "MAGRIB", "ISYA'"};
    const char *sholat = jadwal[sholatNow]; 
    static uint8_t ct = 0;
    static uint32_t lsRn = 0;
    uint32_t Tmr = millis();
    const uint8_t limit = config.durasiadzan;

    if (Tmr - lsRn > 500 && ct <= limit)
    {
        lsRn = Tmr;
        if (!(ct & 1))  // Lebih cepat dibandingkan ct % 2 == 0
        {
            fType(0);
            dwCtr(0, 8, "ADZAN");
            dwCtr(0, 16, sholat);
            Buzzer(1);
        }
        else
        {
            Buzzer(0);
        }
        ct++;
        DoSwap = true;
    }
    
    if ((Tmr - lsRn) > 1500 && (ct > limit))
    {
        show = ANIM_IQOMAH;
        ct = 0;
        Buzzer(0);
    }
}

void drawIqomah()  // Countdown Iqomah (9 menit)
{  
    static uint32_t lsRn = 0;
    static uint16_t ct = 0;  // Mulai dari 0 untuk menghindari error
    static uint8_t mnt, scd;
    char locBuff[10];  
    uint32_t now = millis();  // Simpan millis() di awal
    
    uint16_t cn_l = (iqomah[sholatNow] * 60);
    
    mnt = (cn_l - ct) / 60;
    scd = (cn_l - ct) % 60;
    snprintf(locBuff,sizeof(locBuff), "-%02d:%02d", mnt, scd);

   
    fType(0);
    dwCtr(0, 8, "IQOMAH");
    dwCtr(0, 16, locBuff);
    DoSwap = true;

    if (now - lsRn > 1000) 
    {   
        lsRn = now;
        ct++;

        if (ct > (cn_l - 5)) {
            Buzzer(ct & 1);  // Gunakan bitwise untuk menggantikan modulo 2
        }
    }

    if (ct >= cn_l)  // Pakai >= untuk memastikan countdown selesai dengan benar
    {
        ct = 0;
        Buzzer(0);
        show = ANIM_BLINK;
    }    
}

void blinkBlock()
{
    static uint32_t lsRn;
    static uint16_t ct = 0;
    const uint16_t ct_l = displayBlink[sholatNow] * 60;  // Durasi countdown
    uint32_t now = millis();  // Simpan millis()

    // Ambil waktu dari RTC
    RtcDateTime rtcNow = Rtc.GetDateTime();

    // Hitung countdown
    uint8_t mnt = (ct_l - ct) / 60;
    uint8_t scd = (ct_l - ct) % 60;

    // Tampilkan jam besar
    char timeBuff[9];
    sprintf(timeBuff, "%02d:%02d:%02d", rtcNow.Hour(), rtcNow.Minute(),rtcNow.Second());
    
    fType(3);
    dwCtr(0, 16, timeBuff);
    DoSwap = true;

    // Update countdown setiap detik
    if (now - lsRn > 1000)
    {
        lsRn = now;
        ct++;
    }

    // Reset jika countdown selesai & kembali ke animasi utama
    if (ct > ct_l)
    {
        sholatNow = -1;
        adzan = false;
        ct = 0;
        show = ANIM_JAM;
//        logo1(48);
//        logo2(0);
    }
}



void logo1 (uint8_t x){
  //if (adzan) return;
  static const uint8_t logo1[] PROGMEM = {
    16,16,
  0x00, 0x00, 0x0c, 0xc0, 0x0d, 0xc0, 0x19, 0xc1, 0x00, 0x03, 0x04, 0x0b, 0x4c, 0xdb, 0x9c, 0xdb, 0xbc, 0xdb, 0xfc, 0xdb, 0x6c, 0xdb, 0x0c, 0xdb, 0x0c, 0xdb, 0x0f, 0xfb, 0x07, 0x32, 0x00, 0x00

    //0x06, 0x60, 0x06, 0xe3, 0x0c, 0xe3, 0x00, 0x01, 0x02, 0x05, 0x06, 0x6d, 0x4e, 0x6d, 0x5e, 0x6d, 0x7e, 0x6d, 0x36, 0x6d, 0x06, 0x6d, 0x06, 0x6d, 0x06, 0x6d, 0x07, 0xfd, 0x03, 0x98, 0x00, 0x00
  };
  Disp.drawBitmap(x,-1,logo1);
  //DoSwap = true;
}

void logo2 (uint8_t x){
  //if (adzan) return;
  static const uint8_t logo2[] PROGMEM = {
    16,16,
    0x00, 0x00, 0x13, 0x00, 0x1b, 0x00, 0x18, 0x38, 0x08, 0x2c, 0x0c, 0x78, 0x0d, 0xf0, 0x07, 0x00, 0x07, 0xff, 0x0c, 0x7c, 0x1d, 0xe0, 0x77, 0x80, 0xe3, 0x80, 0x83, 0x80, 0x01, 0x80, 0x00, 0x00
};
  Disp.drawBitmap(x,-1,logo2);
  //DoSwap = true;
}
/*======================================================================================*/
void dwCtr(int8_t x, int8_t y, const char* Msg){
   uint16_t   tw = Disp.textWidth(Msg);
   int16_t   c = int16_t((DWidth-x-tw)/2);
   Disp.drawText(x+c,y,Msg);
}
  
void fType(int8_t x)
  {
    if(x==0) Disp.setFont(Font0);
    else if(x==1) Disp.setFont(Font1); 
    else if(x==2) Disp.setFont(Font2);
    else if(x==3) Disp.setFont(Font3);
    else if(x==4) Disp.setFont(Font4);
  //  else Disp.se
  }
