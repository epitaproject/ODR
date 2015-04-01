typedef struct point
{
	int x;
	int y;
} point;

typedef struct line
{
	point* p1;
	point*p2;
} line;

//soit l la ligne représentant une éqation linéaire f, trace(t) retourne f(t)
int trace(line *l,int t);
 
 //retourne trace(o,t), o étant orthogonal à l (et de même point d'origine)
 int trace_orth(line *l,int t);