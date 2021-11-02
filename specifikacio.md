*Nagy Ábel, CPD 63P* <br>
*Prog1 NHF2,* **Game of Life specifikáció**

# Mi fogadja a felhasználót a program megnyitásakor?
    - Grafikus UI, a felhasználó nem lát konzolt
    - 3 opciós menü: játék, betöltés, súgó
    - A játék menüpont előhoz egy grafikus felületet, ahhol a kezdőállapotot lehet megadni
    - A Betöltés menüpontban egy korábbi kezdőállapotot lehet megnyitni. (Alapértelmezett, vagy korábban felh. által létrehozott.)
    - A súgó menüpont egy rövid szöveges leírást jelenít meg a GoL játékról.

## 1. "Játék"
    - A felhasználó mindenek előtt megadja a játéktér dimenzióit (szélesség, magasság). A felhasználó javaslatot kap a játéktér méretére, az ablak méretétől függően.
    - Kívánt méretű négyzetháló jelenik meg. A cellákra kattintva a játékos váltogathatja, hogy a cellák élők (fekete), vagy halottak (fehér) legyenek a szimuláció kezdetekor.
    - Az ablak szélén egy mentés ikonnal lehetőség van elmenteni a játéktér jelenlegi állapotát. Ez a játéktér létrzése közben végig jelen van. Erre kattintáskor a felhasználó nevet kell adjon a mentésének.
    - Az ablak szélén van egy lejátszás gomb. Ennek megnyomása után elindul a szimuláció futtatása. A gomb megnyomása után a gomb átváltozik megállít gombbá, ami megállítja a szimuláció futását. A futó szimulációban az állapotok rövid szünettel váltakoznak.
    - Az ablak szélén a lejátszik/megállít gomb közelében van egy "következő állapot" gomb. Ez betölti a szimuláció következő állapotát.
    - Az escape gombe megnyomásával vissza lehet térni a menübe.


## 2. "Betöltés"
    - Megnézi milyen fájlok vannak a ./saves mappában, ezekből felajánl egy listát, ahonnan a felhasználó kiválaszthat egy mentést, amit betölt. A listában a mentések neve jelenik meg.
    - A listában megjelenik néhány alapértelmezett állapot, amelyek a Wikipédia oldal általi páldák alapján készültek.
    - A betöltött játék megnyitja a játéktért, ahhol a cellák olyan állapotban vannak mint a mentés pillanatában voltak.
    - Betöltés előtt a játék ellenőrzi a betöltendő mentés formai helyességét, hibaüzenetet dob ha az nem helyes.
    - Az escape gombe megnyomásával vissza lehet térni a menübe.

## 3. "Súgó"
    - A súgó kiírja a GoL működésének szabályait és keletkezésének történetét, a Wikipédia oldal segítségével.
    - Mutat egy képet John Conwayról, rest in peace üzenettel

# A mentett játékállások
A mentett játékállapotok txt fájlok, a játék mappáján belüli ./saves mappában.<br>
Az adatok az alábbi formátumban tárolódnak: (A 0-s jegyek az élettelen, az 1-esek az élő cellákat jelzik.)
```
{verziószám}
{szélesség} {magasság}
0 0 0 0 0
0 0 1 0 0
0 0 1 0 0
0 0 1 0 0
0 0 0 0 0