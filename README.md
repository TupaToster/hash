# Хэш-таблица

## О чем данная работа
В данной работе исследуется скорость работы [хэш-таблицы](https://en.wikipedia.org/wiki/Hash_table) в зависимости от использованных [хэш-функций](https://en.wikipedia.org/wiki/Hash_function) и различных оптимизаций. В первой части работы будет исследован load-factor таблицы и его дисперсия. Во второй части будут произведены некоторые оптимизации хэш-таблицы, и будут оценены их эффективности.

# Исследование различных хэш-функций
Здесь мы исследуем эффективность следующих хэш функций:
1) oneHash - всегда возвраает 1
2) firstAsciiHash - возвращает код первого символа
3) lenHash - возвращает длину переданной строки
4) asciiSumHash - возвращает сумму всех символов
5) rolHash - считает итерацию хэша следующим способом: [rol](https://sysprog.ru/post/komandy-rol-i-ror)(hash) + str[i]
6) rorHash - аналогична предыдущей, но использует функцию [ror](https://sysprog.ru/post/komandy-rol-i-ror)
7) crc32Hash - эта функция использует алгоритм [crc32](https://en.m.wikipedia.org/wiki/Computation_of_cyclic_redundancy_checks) без особых изменений

## oneHash

Данная фукция выглядит следующим образом
```c++
hash_t one (char* str) {

    return 1;
}
```

Она дает следующий график распределения:

![oneHash](oneHash.png)

| Фактор загрузки | Дисперсия |
| --- | --- |
|7710 | 0 |

## firstAsciiHash

```c++
hash_t firstAscii (char* str) {

    assert (str != NULL);

    return *str;
}
```
![firstAsciiHash](firstAsciiHash.png)

| Фактор загрузки | Дисперсия |
| --- | --- |
| 145.47 | 0 |

## lenHash

```c++
hash_t lenHash (char* str) {

    assert (str != NULL);

    return strlen (str);
}
```
![lenHash](lenHash.png)

| Фактор загрузки | Дисперсия |
| --- | --- |
| 405.79 | 0 |

## asciiSum

```c++
hash_t asciiSum (char* str) {

    assert (str != NULL);

    hash_t retVal = 0;

    while (*str != '\0') {

        retVal += (unsigned char) *str;
        str++;
    }

    return retVal;
}
```
![asciiSumHash](asciiSumHash.png)

| Фактор загрузки | Дисперсия |
| --- | --- |
| 6.75 | 0 |

## rolHash
```c++
hash_t rol (hash_t val) {

    hash_t bit = val & (1 << (8 * sizeof (val) - 1));
    bit >>= (sizeof (val) * 8 - 1);
    val <<= 1;
    val |= bit;
    return val;
}

hash_t rolHash (char* str) {

    assert (str != NULL);

    hash_t retVal = 0;

    for (;*str != '\0'; str++)
        retVal = rol (retVal) + *str;

    return retVal;
}
```
![rolHash](rolHash.png)

| Фактор загрузки | Дисперсия |
| --- | --- |
| 5.27 | 0 |

## rorHash
```c++
hash_t ror (hash_t val) {

    hash_t bit = val & 1;
    val >>= 1;
    val |= (bit << (8 * sizeof (val) - 1));
    return val;
}

hash_t rorHash (char* str) {

    assert (str != NULL);

    hash_t retVal = 0;

    for (;*str != '\0';str++)
        retVal = ror (retVal) + *str;

    return retVal;
}
```
![rorHash](rorHash.png)

| Фактор загрузки | Дисперсия |
| --- | --- |
| 5.28 | 0 |

## crc32Hash
```c++
hash_t crc32Tab[256] = {0};
bool filled = false;
const hash_t crc32Poly = 0xEDB88320;

void fillCrc32 () {

    filled = true;

    for (int i = 0; i < 256; i++) {

        crc32Tab[i] = i;
        for (int j = 0; j < 8; j++)
            crc32Tab[i] = (crc32Tab[i] & 1) ? (crc32Tab[i] >> 1) ^ crc32Poly : (crc32Tab[i] >> 1);

    }
}

hash_t crc32Hash (char* str) {

    assert (str != NULL);

    if (!filled) fillCrc32 ();

    hash_t retVal = -1;

    while (*str != '\0')
        retVal = crc32Tab[(unsigned char) retVal ^ (*(str++))] ^ (retVal >> 8);

    return ~retVal;
}
```
![crc32Hash](crc32Hash.png)

| Фактор загрузки | Дисперсия |
| --- | --- |
| 5.24 | 0 |

## Сравнение результатов

Для дальнейших экспериментов возьмем функции rol hash, ascii sum hash и crc32 hash по причине совпадения их масштаба и представим их на одном графике (ror hash не был взят по причине схожести с rol)
![Combined image](combo.png)

На данном изображении явно видно, что наиболее равномерное распределение имеет crc 32, слегка худшее - rol hash, и наихудшее (с пиком в 25 элементов) - ascii sum.

Для следующей части работы будем использовать rol hash как наилучшую из простых функций.

# Оптимизация работы таблицы

## Нахождение узких мест производительности

Тест производительности будет осуществляться следующим образом:
1) Будет замеряться время работы программы в тактах процессора на поиск случайной строки в 32 символа 1000000 раз.
2) Будет произведен запуск с callgrind - ом для определения наиболее расходной функции

Без оптимизаций получаем следующие данные:

|Среднее время работы поиска в тактах| Коеффициент ускорения |
|---|---|
|199694934.18| 1.000 |


Вывод callgrind - а:

![Callgrind output](callgrindOutOp0.png)

### Оптимизация 1: переписывание функции на ассемблере

Как мы видим, наиболее затратной оказывается функция хэширования. Перепишем её на асме в надежде достичь лучших рещультатов.

```asm
section .text
        global _rolHash

; only arg is a pointer to null terminated string, return value - hash value
_rolHash:
                xor rax, rax    ; zeroing hash counter

                push rbx        ; saving rbx val cause it will be used as temp reg
                xor rbx, rbx

loop:
                rol eax, 1              ; rols hash val
                mov bl, byte [rdi]
                add eax, ebx

                inc rdi                 ; next byte
                cmp byte [rdi], 0       ; if zero - break
                jne loop

                pop rbx

                ret
```

Для замены обычного rolHash на _rolHash необходимо определить op1 в начале main.cpp. Проведем измерения производительности после данных изменений.

|Среднее время работы поиска в тактах| Коеффициент ускорения |
|---|---|
| 224845983.18 | 0.888 |

К счастью, программа написанная мной на асме не смогла обогнать -О3, поэтому коеффициент ускорения получился меньше одного.

Вывод callgrind-а

![callgrind out op1](callgrindOutOp1.png)

Хотя по callgrind-у получается что вес функции стал слегка меньше.


