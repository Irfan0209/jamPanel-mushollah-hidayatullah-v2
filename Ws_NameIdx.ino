char * showTanggal(){
  static char buff_date[70];

   RtcDateTime now = Rtc.GetDateTime();
    
  snprintf(
      buff_date,sizeof(buff_date),
      "%s %s, %02d-%02d-%04d %02d %s %02dH",
      Hari[now.DayOfWeek()],
      pasar[jumlahhari() % 5],
      now.Day(), now.Month(), now.Year(),
      Hijir.getHijriyahDate,
      namaBulanHijriah[Hijir.getHijriyahMonth - 1],
      Hijir.getHijriyahYear
    );

    return buff_date;
}
