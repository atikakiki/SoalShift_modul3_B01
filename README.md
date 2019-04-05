# SoalShift_modul3_B01

## Soal 1
1. Buatlah program C yang bisa menghitung faktorial secara parallel lalu menampilkan hasilnya secara berurutan
Contoh:
    ./faktorial 5 3 4
    3! = 6
    4! = 24
    5! = 120
### Penjelasan
Untuk melakukan perhitungan secara paralel, digunakan thread. Pada soal ini jumlah thread yang digunakan adalah sebanyak n (banyaknya bilangan yang diinputkan). Jadi, satu thread menangani satu perhitungan faktorial.

* Membuat fungsi faktorial dalam C pada umumnya 
```
void *factorial(void *a){
        long long int fact=1,j;
        long long int x= (long long int) a;
                for(j=1;j<=x;j++){
                fact=fact*j;
                }
        printf("%lld! = %lld\n",x, fact);
}
```
Tipe dan parameter fungsi factorial dibuat seperti di atas agar dapat dipanggil ketika membuat thread.

* Melakukan scan input. Input pada program ini ada int num[] yang digunakan untuk menyimpan angka yang diinputkan, dan char n yang digunakan untuk mengetahui apakah input sudah mencapai karakter enter (\n). Pada soal tidak ditentukan berapa jumlah angka yang diinputkan, hanya diketahui jika input sampai karakter enter. Int c digunakan sebagai counter.
```
        for(i=0;n!='\n';i++){
                scanf ("%d%c", &num[i], &n);
                c++;
        }
```

* Mengurutkan bilangan dalam array input. Soal meminta agar hasil faktorial ditampilkan dari yang terkecil ke terbesar. Untuk itu diperlukan sorting. Pada program ini digunakan bubble sort.
```
         if(num[i]>num[j]){
                        a=num[i];
                        num[i]=num[j];
                        num[j]=a;
```

* Membuat thread untuk setiap angka dalam array.
```
     for(i=0;i<c;i++){
        long long int m=num[i];
        pthread_create(&T[i], NULL, factorial, (void*) m);
        pthread_join(T[i], NULL);
        }
```
####[Source Code Program No 1](https://github.com/atikakiki/SoalShift_modul3_B01/blob/master/soal1.c)
