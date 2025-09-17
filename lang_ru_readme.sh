#!/bin/bash
#Для перевода программы:
lupdate Schoolar.pro -ts lang_ru.ts
#запускаем QT linguist и редактируем lang_ru.ts
linguist lang_ru.ts
lrelease lang_ru.ts
