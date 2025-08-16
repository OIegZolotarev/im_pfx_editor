
typedef struct ref_params_s
{
    Vector simvel;
}ref_params_t;

typedef struct hud_s
{
    ref_params_t* pparams;
}hud_t;

extern hud_t gHUD;