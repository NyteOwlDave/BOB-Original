
/*
	proto.h.H

	Function prototypes

*/

#ifndef PROTO_DEFINED
#define PROTO_DEFINED


#include "defs.h"

/*--------------------------------------------------------------------------*/

// hacks.c

#include "hacks.h"


/*--------------------------------------------------------------------------*/

// math-vector.c

extern Flt VecNormalize(Vec vec);
extern void identity(Matrix mat);
extern void matrix_cat(Matrix m1, Matrix m2, Matrix dest);
extern void trans_vector(Matrix mat, Vec in, Vec out);
extern void trans_normal(Matrix mat, Vec in, Vec out);
extern void matrix_inverse(Matrix in, Matrix out);
extern void matrix_copy(Matrix m1, Matrix m2);

/*--------------------------------------------------------------------------*/

// math-noise.c

extern void init_noise(void);
extern Flt noise1(Vec p);
extern void noise3(Vec p, Flt* v);
extern Flt turb1(Vec p, int lvl);
extern void turb3(Vec p, Flt* v, int lvl);
extern void DNoise(Vec p, Flt* v);

/*--------------------------------------------------------------------------*/

// csg-bound.c

extern void BuildBoundingSlabs(void);
extern void FindAxis(Object* top, long count);
extern int SortAndSplit(Object** handle, long count);


/*--------------------------------------------------------------------------*/

// csg-clip.c

extern void bound_opt(Object* obj);
extern BOOL clip_check(Clip* head, Vec P);


/*--------------------------------------------------------------------------*/

// prompt.c

// Starts interactive mode (command prompt)
extern void InteractiveConsole();

// Displays a function incomplete message
extern void incomplete(LPCSTR fname);

// Saves an error code an returns the interactive mode
extern void last_call(int return_code);


/*--------------------------------------------------------------------------*/

// system.c

// Invokes the command shell and passes in a command line
extern void Shell(LPCSTR command);

// Invokes the command shell's DIR command
extern void Dir(LPCSTR path);

// Invokes the command shell's CLS command
extern void Cls();

// Invokes the command shell's PAUSE command
extern void Pause();

// Verifies that a file exists
extern BOOL FileExists(LPCSTR szPath);

// Verifies that a folder exists
extern BOOL FolderExists(LPCSTR szPath);

// Opens an HTML help document for a topic
// NOTE: BROKEN...
extern void OpenHelpFile(LPCSTR topic);

// Displays the contents of a text file
extern void ListFile(LPCSTR path);

// Routines to print various common data types
extern void Print(LPCSTR msg);
extern void PrintFlag(LPCSTR msg, BOOL flag);
extern void PrintText(LPCSTR msg, LPCSTR txt);
extern void PrintTextArray(LPCSTR msg, LPCSTR* pa);
extern void PrintInteger(LPCSTR msg, int num);
extern void PrintFloat(LPCSTR msg, double num);
extern void PrintHex(LPCSTR msg, DWORD num);
extern void PrintVector(LPCSTR msg, Vec v);
extern void PrintMatrix(LPCSTR msg, Matrix m);
extern void PrintAddress(LPCSTR msg, const void* p);


/*--------------------------------------------------------------------------*/

// windows.c

/*
extern void Popup(LPCSTR sz);
extern void PopupWarning(LPCSTR sz);
extern BOOL PopupConfirm(LPCSTR sz);
*/


/*--------------------------------------------------------------------------*/

// general-main.c

extern int main(int argc, char** argv);
extern void ShootScene(void);


/*--------------------------------------------------------------------------*/

// general-file.c

extern void init_env(void);
extern FILE* env_fopen(LPCSTR filename, LPCSTR mode);
extern PSTRING CompletePath(LPCSTR filename);
extern BOOL ReadMapFile(LPCSTR filename, PBACKGROUND bgnd);
extern void print_search_paths(void);


/*--------------------------------------------------------------------------*/

// general-memory.c

extern void* vmalloc(int size);


/*--------------------------------------------------------------------------*/

// general-stats.c

extern void statistics(int line);
// extern void stat_tic(void);
// extern void init_tic(void);


/*--------------------------------------------------------------------------*/

// general-data.c

extern void print_ray_vars(void);
extern void print_screen_vars(void);
extern void print_session_vars(void);
extern void print_studio_vars(void);
extern void print_light_vars(void);
extern void print_scene_vars(void);
extern void print_surface_vars(void);
extern void print_transform_vars(void);
extern void print_misc_vars(void);
extern void print_slab_vars(void);
extern void print_clip_vars(void);
extern void print_bgnd(void);


/*--------------------------------------------------------------------------*/

// shape-cone.c

extern int ConeIntersect(Object* obj, Ray* ray, Isect* hit);
extern void ConeNormal(Object* obj, Isect* hit, Point P, Vec N);
extern Object* MakeCone(Point basepoint, Flt baseradius, Point apexpoint, Flt apexradius);


/*--------------------------------------------------------------------------*/

// shape-null.c

extern int NullIntersect(Object* obj, Ray* ray, Isect* hit);
extern void NullNormal(Object* obj, Isect* hit, Point P, Vec N);

#define NULLINTERSECT() NullIntersect(NULL,NULL,NULL)
#define NULLNORMAL() NullIntersect(NULL,NULL,NULL,NULL)


/*--------------------------------------------------------------------------*/

// shaqpe-poly.c

extern int PolyIntersect(Object* obj, Ray* ray, Isect* hit);
extern void	PolyNormal(Object* obj, Isect* hit, Point P, Vec N);
extern Object* MakePoly(int npoints, Vec *points);


/*--------------------------------------------------------------------------*/

// shape-ring.c

extern int RingIntersect(Object* obj, Ray* ray, Isect* hit);
extern void RingNormal(Object* obj, Isect* hit, Point P, Vec N);
extern Object* MakeRing(Vec pos, Vec norm, Flt min_rad, Flt max_rad);


/*--------------------------------------------------------------------------*/

// shape-sphere.c

extern int SphereIntersect(Object* obj, Ray* ray, Isect* hit);
extern void SphereNormal(Object* obj, Isect* hit, Point P, Vec N);
extern Object* MakeSphere(Vec pos, Flt radius, Flt fuzzy);


/*--------------------------------------------------------------------------*/

// shape-tri.c

extern int TriIntersect(Object* obj, Ray* ray, Isect* hit);
extern void TriNormal(Object* obj, Isect* hit, Point P, Vec N);
extern Object* MakeTri(Vec* point);
extern void CheckTri(Vec* point);

extern void InvertMatrix(Vec in[3], Vec out[3]);
extern void ScaleMatrix(Flt scale, Vec in[3], Vec out[3]);


/*--------------------------------------------------------------------------*/

// tracer-pqueue.c

extern void PriorityQueueNull(void);
extern BOOL PriorityQueueEmpty(void);
extern void PriorityQueueInsert(Flt key, Object* obj);
extern void PriorityQueueDelete(Flt* key, Object** obj);


/*--------------------------------------------------------------------------*/

// tracer.screen.c

extern void	Screen(Viewpoint* view, char* picfile, int xres, int yres);


/*--------------------------------------------------------------------------*/

// tracer-inter.c

extern void CheckAndEnqueue(Object* obj, Flt maxdist);
extern BOOL Intersect(Ray* ray, Isect* hit, Flt maxdist, Object* self);


/*--------------------------------------------------------------------------*/

// tracer-pic.c

#include "pic.h"
extern Pic* PicOpen(char* filename, int x, int y);
extern void PicWriteLine(Pic* pic, Pixel* buf);
extern void PicClose(Pic* pic);


/*--------------------------------------------------------------------------*/

// tracer-shade.c

extern void Shade(int level, Flt weight, Vec P, Vec N, Vec I, Isect *hit, Color col, Flt ior);
extern void reflect(Vec I, Vec N, Vec R, Flt dot);
extern int refract(Flt eta, Vec I, Vec N, Vec T, Flt dot);


/*--------------------------------------------------------------------------*/

// tracer-shadow.c

extern int Shadow(Ray* ray, Isect* hit, Flt tmax, Color color, int level, Light* lptr, int inside);
extern int sShadow(Ray* ray, Isect* hit, Flt tmax, Color color, int level, Light* lptr, int inside);


/*--------------------------------------------------------------------------*/

// tracer-texture.c

extern Flt tex_checker(Point P, Texture* tex);
extern Flt tex_spherical(Point P, Texture* tex);
extern Flt tex_noise(Point P, Texture* tex);
extern void tex_fix(Surface* surf, Point P, Point OP);
extern void map_fix(Surface* surf, Point P);
extern void tex_project(Texmap* tm, Point OP, Flt* i, Flt* j);
extern void tile(Texmap* tm, Flt* i, Flt* j);
extern void get_map_entry(Texmap* tm, Flt i, Flt j, Color color);
extern void tex_read_img(char* file, Texmap* tm);


/*--------------------------------------------------------------------------*/

// tracer-trace.c

extern Flt Trace(int level, Flt weight, Ray* ray, Color color, Flt ior, Object* self);
extern void bkg(Vec dir, Color col);


/*--------------------------------------------------------------------------*/

// tracer-wave.c

extern void make_waves(Vec in, Vec out, Wave* head);


/*--------------------------------------------------------------------------*/

// lang-parse.c

extern int yyparse(void);


/*--------------------------------------------------------------------------*/

// lang-extra.c

extern void ReadSceneFile(LPCSTR real, LPCSTR temp);

extern void ptrchk(const void* ptr, LPCSTR str);

extern void yystats(void);
extern void yy_popfile(void);
extern void yy_newfile(LPCSTR file);
extern int yyerror(LPCSTR str);

extern void trans_pop(void);
extern void trans_popall(void);


/*--------------------------------------------------------------------------*/

// lang-preproc.c

extern int preproc(LPCSTR infile, LPCSTR outfile);
extern void print_macros(void);


/*--------------------------------------------------------------------------*/

// lang-string.c

extern int GetStringLength(PSTRING p);

extern PSTRING CreateString(LPCSTR src);
extern void DestroyString(PSTRING p);

extern PSTRING CopyString(PSTRING src);
extern PSTRING AppendString(PSTRING str, LPCSTR add);
extern PSTRING ReverseString(PSTRING str);
extern PSTRING StringToUppercase(PSTRING str);
extern PSTRING StringToLowercase(PSTRING str);

extern int FindSubString(PSTRING src, LPCSTR find);
extern PSTRING LeftSubString(PSTRING src, int count);
extern PSTRING RightSubString(PSTRING src, int count);
extern PSTRING CopySubString(PSTRING src, int first, int count);
extern PSTRING CutSubString(PSTRING src, int first, int count);
extern PSTRING DeleteSubString(PSTRING src, int first, int count);
extern PSTRING InsertSubString(PSTRING src, int first, LPCSTR ins);
extern PSTRING ReplaceSubString(PSTRING src, int first, int count, LPCSTR ins);

extern void PrintString(PSTRING str);

extern char *strrev(char *str);
extern void strupr(char *str);
extern void strlwr(char *str);


/*--------------------------------------------------------------------------*/

// lang-macro.c

extern PMACROEX CreateEmptyMacro(LPCSTR name);
extern PMACROEX CreateMacro(LPCSTR name, LPCSTR text);
extern void DestroyMacro(PMACROEX macro);
extern void SetMacroText(PMACROEX macro, LPCSTR text);

extern void AddMacro(PMACROEX macro);
extern void RemoveMacro(PMACROEX macro);
extern void RemoveAllMacros();
extern PMACROEX FindMacro(LPCSTR name);

extern void PrintMacro(LPCSTR name);
extern void PrintMacros();


/*--------------------------------------------------------------------------*/

// lang-tokens.c

// See: tokens.h


/*--------------------------------------------------------------------------*/

// log.c

extern void LogClose(void);
extern void LogOpen(void);
extern void LogPrint(LPCSTR msg);
extern void LogToken(int token);


#endif // !PROTO_DEFINED

