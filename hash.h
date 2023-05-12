#include "flog.h"

typedef unsigned int hash_t;

#define dump(clas) (clas).dumpInside (#clas, __FILE__, __FUNCTION__, __LINE__)

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
    const size_t cap = 7;
    hash_t (*strHash) (char* str) = NULL;
    Nod* data = NULL;

public:
    HashTable (hash_t (*_strHash) (char* str), Nod* _data = NULL) : strHash (_strHash), data (_data) {

        if (_data != NULL) return;

        data = (Nod*) calloc (cap, sizeof (Nod));
        assert (data != NULL);

        for (int i = 0; i < cap; i++) {

            data[i] = Nod ();
            data[i].next = data + i;
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

        return _find (key)->val;
    }

    Nod* _find (char* key) {

        assert (key != NULL);

        hash_t hash = strHash (key);

        for (Nod* iter = data[hash % cap].next; iter != data + hash % cap; iter=iter->next)
            if (iter->hash == hash and strcmp (key, iter->key) == 0)
                return iter;

        return data + hash % cap;
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

        char command[40] = "";
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
};