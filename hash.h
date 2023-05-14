#include "flog.h"

typedef unsigned int hash_t;

inline hash_t defaultHash (char* str) {

    assert (str != NULL);

    return (hash_t) countHash (str, str + strlen (str));
}

#ifndef NDEBUG
#define dump(clas) (clas).dumpInside (#clas, __FILE__, __FUNCTION__, __LINE__)
#else
#define dump(clas) ;
#endif

class HashTable {

public:

    struct Nod {

        Nod*    next    = NULL;
        char*   key     = NULL;
        hash_t  hash    = 0;
        int     val     = 0;

        Nod (hash_t (*strHash) (char* key) = NULL, char* _key = NULL, int _val = 0, Nod* _next = NULL) : next (_next), val (_val), key (_key) {

            if (key == NULL or strHash == NULL) return;

            key = strdup (_key);
            assert (key != NULL);

            hash = strHash (key);
        }

        void DTOR () {

            setPoison (next);
            if (val != NULL) free (key);
            setPoison (key);
            setPoison (val);
        }
    };

private:
    static const int NOT_FOUND = 0xBADC0FEE;
    static const size_t defaultCap = 71089;
    size_t cap = 0;
    hash_t (*strHash) (char* str) = NULL;
    Nod* data = NULL;

public:
    HashTable (hash_t (*_strHash) (char* str) = defaultHash, size_t _cap = defaultCap) : strHash (_strHash), cap (_cap) {

        assert (strHash != NULL);

        data = (Nod*) calloc (cap, sizeof (Nod));
        assert (data != NULL);

        for (int i = 0; i < cap; i++) {

            data[i] = Nod ();
            data[i].next = data + i;
        }
    }


    // Please don't change hash func between saving and loading back up
    HashTable (const char* fileName, hash_t (*_strHash) (char* str) = defaultHash) {

        assert (fileName != NULL);

        FILE* inFile = fopen (fileName, "rb");
        assert (inFile != NULL);

        int readFlag = 0;

        #define scn(...) {readFlag = fscanf (inFile, __VA_ARGS__); assert (readFlag != 0); }

        scn ("%llu ", &cap);

        HashTable (_strHash, cap);

        for (int i = 0; i < cap; i++) {

            scn ("[%d]", &data[i].val);

            for (int j = 0; j < data[i].val; j++) {

                Nod* newNod = (Nod*) calloc (1, sizeof (Nod));
                assert (newNod != NULL);

                scn ("->[%[^$]:%d]", newNod->key, &newNod->val);
            }
        }
    }

    void DTOR () {

        assert (data != NULL);

        for (int i = 0; i < cap; i++)
            for (Nod* iter = data + i; iter != data + i;) {

                Nod* nxt = iter->next;
                iter->DTOR ();
                iter = nxt;
            }


        free (data);
        setPoison (data);
        setPoison (strHash);
    }

    inline int find (char* key) {

        Nod* retVal = _find (key);
        if (retVal->key == NULL) return NOT_FOUND;
        return retVal->val;
    }

    Nod* _find (char* key) {

        assert (key != NULL);

        hash_t hash = strHash (key);

        for (Nod* iter = data[hash % cap].next; iter != data + hash % cap; iter=iter->next)
            if (iter->hash == hash and strcmp (key, iter->key) == 0)
                return iter;

        return data + hash % cap;
    }

    void del (char* key) {

        assert (key != NULL);

        Nod* iter = _find (key);
        if (iter->key == NULL) return;

        Nod* prev = iter->next;
        Nod* base = NULL;

        for (;prev->next != iter; prev = prev->next) {

            if (prev->key == NULL) base = prev;
        }
        if (prev->key == NULL) base = prev;

        assert (base != NULL);

        prev->next = iter->next;
        base->val--;

        iter->DTOR ();
    }

    Nod* insert (char* key, int val = 0) {

        assert (key != NULL);

        Nod* iter = _find (key);
        if (iter->key == NULL) { // This means that this key doesn't exist in table and iter is pointing to the zero element in cell data[hash % cap]

            Nod* newNod = (Nod*) calloc (1, sizeof (Nod));
            assert (newNod != NULL);

            *newNod = Nod (strHash, key, val, iter->next);

            iter->next = newNod;
            iter->val++;
        }
        else {

            iter->val = val;
        }

        return iter;
    }

    void graphDump () {

        static int GraphDumpCounter = 0;

        if (GraphDumpCounter == 0) system ("rm *.png");

        #define picprintf(...) fprintf (picSource, __VA_ARGS__);

        char srcName[] = "GraphDumpSrc.dot";
        FILE* picSource = fopen (srcName, "w");
        setvbuf (picSource, NULL, _IONBF, 0);


        char picName[30] = "";
        sprintf (picName, "GraphDump_%d.png", GraphDumpCounter);

        picprintf ("digraph HashTab_%d {" "\n", GraphDumpCounter);
        picprintf ("\t" "graph [dpi = 200];" "\n");
        picprintf ("\t" "bgcolor = \"#252525\"" "\n");
        picprintf ("\t" "rankdir = LR" "\n");

        for (int i = cap - 1; i >= 0; i--) {

            picprintf ("\t" "\"Nod_%d_0\" [shape = \"Mrecord\", style = \"filled\", fillcolor = \"#1ed5f2\", label = \" Type = &lt;ODD&gt; | <line> Bucket = &lt;%u&gt; | List Size = &lt;%d&gt;\"]\n", i, i, data[i].val);

            int cnt = 1;
            for (Nod* iter = data[i].next; iter != data + i; iter = iter->next) {

                picprintf ("\t" "\"Nod_%d_%d\" [shape = \"Mrecord\", style = \"filled\", fillcolor = \"#1ed5f2\", label = \" Type = &lt;DATA&gt; | <line> Key = &lt;%s&gt; | Val = &lt;%d&gt;\"]\n", i, cnt, iter->key, iter->val);
                cnt++;
            }
        }

        for (int iter = 0;;iter++) {

            bool flag = true;

            for (int i = 0; i < cap; i++) {

                if (data[i].val >= iter) {
                    if (flag) {

                        picprintf ("\t" "{rank = same; ");
                        flag = false;
                    }
                    picprintf ("Nod_%d_%d; ", i, iter);
                }
            }

            if (flag) break;

            picprintf ("}\n");
        }

        for (int i = 0; i < cap; i++) {

            int cnt = 1;
            for (Nod* iter = data[i].next; iter != data + i; iter = iter->next) {

                picprintf ("\t" "\"Nod_%d_%d\":line -> \"Nod_%d_%d\":line [color = \"#36f70f\"];\n", i, cnt - 1, i, cnt);
                cnt++;
            }
        }


        picprintf ("}");

        fclose (picSource);

        char command[100] = "";
        sprintf (command, "dot -Tpng %s -o %s", srcName, picName);

        system (command);

        flogprintf ("<h2>Tree dump</h2><br>");
        flogprintf ("<img src = \"%s\"><br>", picName);

        GraphDumpCounter++;

        #undef picprintf
    }

    void dumpInside (const char* name = NULL, const char* fileName = NULL, const char* funcName = NULL, size_t line = 0, void (*dumpFunc) (Nod* obj) = NULL) {

        flogprintf ("<pre>" "In file %s, function %s, line %llu, HashTable named \"%s\" was dumped :<br>",
                    fileName, funcName, line, name);

        flogprintf ("\tcap  = %llu\n", cap);
        flogprintf ("\tdata = %p\n", data);

        graphDump ();

        flogprintf ("</pre><hr>\n");
    }

    void save2file (const char* fileName) {

        assert (fileName != NULL);

        FILE* outFile = fopen (fileName, "wb");
        assert (outFile != NULL);

        #define oprintf(...) fprintf (outFile, __VA_ARGS__);

        oprintf ("%u\n", cap);

        for (int i = 0; i < cap; i++) {

            oprintf ("[%d]", data[i].val);

            for (Nod* iter = data[i].next; iter != data + i; iter = iter->next){

                oprintf ("->[%s$:%d]", iter->key, iter->val);
            }

            oprintf ("\n");
        }

        fclose (outFile);

        #undef oprintf
    }
};