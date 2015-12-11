### Functions ###

```
query(label, type, initial_value=None, ok=None, cancel=None)
```

This function is already known from the appuifw module. It performs a query with a single-line dialog. For the usage, refer to the PyS60 documentation. New in this version are the `ok` and `cancel` arguments which may be used to replace the default softkeys texts (unicode).

New in version 1.0.1.


---


```
get_language()
```

Returns the ID of the current language.

The known languages are:
```
ELangTest # Value used for testing - does not represent a language.
ELangEnglish # UK English.
ELangFrench # French.
ELangGerman # German.
ELangSpanish # Spanish.
ELangItalian # Italian.
ELangSwedish # Swedish.
ELangDanish # Danish.
ELangNorwegian # Norwegian.
ELangFinnish # Finnish.
ELangAmerican # American.
ELangSwissFrench # Swiss French.
ELangSwissGerman # Swiss German.
ELangPortuguese # Portuguese.
ELangTurkish # Turkish.
ELangIcelandic # Icelandic.
ELangRussian # Russian.
ELangHungarian # Hungarian.
ELangDutch # Dutch.
ELangBelgianFlemish # Belgian Flemish.
ELangAustralian # Australian English.
ELangBelgianFrench # Belgian French.
ELangAustrian # Austrian German.
ELangNewZealand # New Zealand English.
ELangInternationalFrench # International French.
ELangCzech # Czech.
ELangSlovak # Slovak.
ELangPolish # Polish.
ELangSlovenian # Slovenian.
ELangTaiwanChinese # Taiwanese Chinese.
ELangHongKongChinese # Hong Kong Chinese.
ELangPrcChinese # Peoples Republic of China's Chinese.
ELangJapanese # Japanese.
ELangThai # Thai.
ELangAfrikaans # Afrikaans.
ELangAlbanian # Albanian.
ELangAmharic # Amharic.
ELangArabic # Arabic.
ELangArmenian # Armenian.
ELangTagalog # Tagalog.
ELangBelarussian # Belarussian.
ELangBengali # Bengali.
ELangBulgarian # Bulgarian.
ELangBurmese # Burmese.
ELangCatalan # Catalan.
ELangCroatian # Croatian.
ELangCanadianEnglish # Canadian English.
ELangInternationalEnglish # International English.
ELangSouthAfricanEnglish # South African English.
ELangEstonian # Estonian.
ELangFarsi # Farsi.
ELangCanadianFrench # Canadian French.
ELangScotsGaelic # Gaelic.
ELangGeorgian # Georgian.
ELangGreek # Greek.
ELangCyprusGreek # Cyprus Greek.
ELangGujarati # Gujarati.
ELangHebrew # Hebrew.
ELangHindi # Hindi.
ELangIndonesian # Indonesian.
ELangIrish # Irish.
ELangSwissItalian # Swiss Italian.
ELangKannada # Kannada.
ELangKazakh # Kazakh.
ELangKhmer # Khmer.
ELangKorean # Korean.
ELangLao # Lao.
ELangLatvian # Latvian.
ELangLithuanian # Lithuanian.
ELangMacedonian # Macedonian.
ELangMalay # Malay.
ELangMalayalam # Malayalam.
ELangMarathi # Marathi.
ELangMoldavian # Moldovian.
ELangMongolian # Mongolian.
ELangNorwegianNynorsk # Norwegian Nynorsk.
ELangBrazilianPortuguese # Brazilian Portuguese.
ELangPunjabi # Punjabi.
ELangRomanian # Romanian.
ELangSerbian # Serbian.
ELangSinhalese # Sinhalese.
ELangSomali # Somali.
ELangInternationalSpanish # International Spanish.
ELangLatinAmericanSpanish # American Spanish.
ELangSwahili # Swahili.
ELangFinlandSwedish # Finland Swedish.
ELangReserved1 # Reserved for future use.
ELangTamil # Tamil.
ELangTelugu # Telugu.
ELangTibetan # Tibetan.
ELangTigrinya # Tigrinya.
ELangCyprusTurkish # Cyprus Turkish.
ELangTurkmen # Turkmen.
ELangUkrainian # Ukrainian.
ELangUrdu # Urdu.
ELangReserved2 # Reserved for future use.
ELangVietnamese # Vietnamese.
ELangWelsh # Welsh.
ELangZulu # Zulu.
ELangOther # Use of this value is deprecated.
```


---


```
get_skin_color(color_id)
```

Returns an `(r, g, b)` tuple representing a color defined by the currently selected skin.

The defined color IDs are:
```
EMainAreaTextColor # used by skinned Text UI control
```