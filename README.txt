Struktura katalogów oraz autorzy poszczególnych części biblioteki
=================================================================
|- dataset/          - zbiór danych zgromadzony na potrzeby pracy
|                      magisterskiej
|- lib/              - zewnętrzne biblioteki, z których
|                      skorzystano podczas implementacji
|- src/
|--- speech/
|----- clustering/   - klasy implementujące metody klastrowania
|                      (autor: Kacper Łukawski)
|------- exception/  - powiązane klasy wyjątków
|----- detector/     - klasy dostarczające detektory cech
|                      dźwiękowych (autor: Kacper Łukawski)
|----- exception/    - powiązane klasy wyjątków
|----- initializer/  - klasy inicjalizujące model oparty
|                      o leksykon i ukryte łańcuchy Markowa
|                      (autor: Kacper Łukawski)
|----- metric/       - klasy implementujące metryki
|                      (autor: Kacper Łukawski)
|----- raw_data/     - klasy opisujące dane dźwiękowe
|                      w dziedzinie czasu oraz częstotliwości,
|                      a także całe źródła tych danych (autor:
|                      Szymon Wenta)
|------- filtering/  - filtry dźwiękowe operujące na danych
|                      w dziedzinie czasu bądź częstotliwości
|                      (autor: Szymon Wenta)
|----- spelling/     - klasy operujące na słowach zapisanych
|                      ortograficznie
|------- exception/  - powiązane klasy wyjątków
|----- transform/    - implementacje transformat
|                      częstotliwościowych (autor: Szymon Wenta)
|------- window/     - klasy reprezentujące funkcje okna, używane
|                      podczas transformaty częstotliwościowej
|----- vectorizer/   - klasy służące do wektoryzacji próbek
|                      dźwiękowych (autor: Kacper Łukawski)
|--- main.cpp        - program główny opisany w pracy