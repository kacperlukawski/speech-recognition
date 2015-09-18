Struktura katalogów oraz autorzy poszczególnych części biblioteki
=================================================================
|- dataset/          - zbiór danych zgromadzony na potrzeby pracy
|                      magisterskiej
|- lib/              - zewnętrzne biblioteki, z których
|                      skorzystano podczas implementacji
|- src/              - pliki źródłowe biblioteki
|--- speech/
|----- clustering/   - autor: Kacper Łukawski
|----- initializer/  - autor: Kacper Łukawski
|----- metric/       - autor: Kacper Łukawski
|----- raw_data/     - autor: Szymon Wenta
|------- filtering/  - autor: Szymon Wenta
|----- transform/    - autor: Szymon Wenta
|------- window/     - autor: Szymon Wenta
|----- vectorizer/   - autor: Kacper Łukawski
|--- main.cpp        - program główny opisany w pracy

Uruchamianie programu głównego
=================================================================
Program główny należy uruchamiać przekazując ścieżkę do pliku
w formacie JSON, zawierającego konfigurację modelu. Plik
lexicon_conf.example.json zawiera konfigurację wykorzystującą
cały zbiór danych, w sposób opisany w pracy.
