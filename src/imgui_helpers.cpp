/*
    LightBaker3000 Frontend project,
    (c) 2023 CrazyRussian
*/

#include "application.h"
#include "imgui_shared.h"
#include "imgui_helpers.h"

#include "lb3k_imgui_icons.h"
#

#include <imgui_internal.h>

GLTexture *g_pToolbarIconsSheet;


extern ImVec4 g_CommonIconsRects[39];

const char FONT_ICON_BUFFER_NAME_lb3k[2870+1] = 
    "7])#######oNBTn'/###Z),##+Sl##Q6>##w#S+HrR?:hID/2'8XC'#PK[^IU^bI#MnP<-iNE/1-2JuBhtlx41->>#vrEn/a_[FH4F02#R(m<-:qEn/Fd@UC6HcY#6fG<-87T;-QF%U."
    "_W9Q]whG<-^).m/Q-0%Jq,bH@lW+41e3NP&sMV=BQLtpme(m<-14S>-XDRu1_0VuPWbZPAeI1=#2w5tL72pFi$N`1Maf4f0_CL`<auNlfi^l+M*<>GMM%g+MK=6##%xg^#x%P:vWCk(N"
    "U$DeM09iHMkT/@#``ZIMXC6###?x'/_/_f1+%u&#6hFJ(<7UV-`[f0514n-$70ll&=>ur$<7E_&pm]V$r=F&#v[0B#'5>>#2*]p&hrES7(+q>$exdw'2An1v^4`:m#h&gL0FPGM26pfL"
    "$`Q##3Yu##@W,110f1$#<rC$#@(V$#ZpefMp`5oLit,wL*go#M9,F<#q#eiLrf>oLODetL3Gl$M<>b<#t2wiL`#(5#YbT:#>b@%#jg)<#[dE1#^kK:#KH+&#NLk3.@lOoLxCe##Y@2##"
    "HJ8K%G(7B3hq^`jmb%Z#'fX&#rax6*7fx6*`Hbs$f&:;]QbuX6V$nO(]ax9.E';sC4x(cuj+CD3tHE`<VHaoRC7C']%`,AOo4VoINS'##*.r-$xOF&#IkhP0UKO2L6xjY(srwFMf@-##"
    "RFP)M$F`hL<SF&#kF3]-:sE+ieRLg0eiES7ft@`acR/,)jc;F3N#G:.GC#<.9EsI3rM1UofMHVE:FROEQb+Jhe/W]7BasfL8v'kL(x3'M*G?##v-f'0ml*F3*0fX-psHm1#TiB#O5]P#"
    "O3`ah:mZ+SLxSfLa$TfLhRrhL[OZuYL%tILR0Vq2w%bpufjES700D>CMj,UDbcf<CK5M^YN@7Z7fBSp0epLMhr<9K/6Y/E#>P9@MA$TfLMY@IMH%+n-ANB3k2UnhM;YjLF=TrA#mew^M"
    "%Ru_.:_G)46.QXMM@kJ-FQ]b-&[lKYG.8>>4GO#$#Awf(/$uWU4rSa#KXoQ0m$$h-w0u^fdEm9#(&>uu=p,tL6Du[$)KX$#2^*87t$1F*g`<^44s;^4eX]u.M/KF4S1_F*hZbt$X4:W-"
    "i4NT/)2?&@]/(a?4ngM34KYcu&KlN'mFgJ)nFo+M-oQ=ld%@^5$Vvbr=@Mda=,$*NVtI=(JO&pA,ltA#F5n0#PWrQNNA^cD4.')*3V3>5$),##0lAGM6l8,M^NmhLYw(.MVflS.BXD8v"
    "]W^C-%vo(%e*gi'_.Hq;3]f+M%xSfLfP?uu,j-4vVaM8vh(vW-]vJe$YFNP&D#p-$U+RS%$iLP/l8a@t)JOfCpr%)t_+st(>W<>,A'KM'-L(F7c/_:mO7Wiq<T-B,tmR:v*=<8o$bKwK"
    ")YTk+NY7R3B:+_JR(1wgt.fc)<eoY6jZ/W-nX#:D?u%FI_Eb9DRXNVeZ-U-H;(Xh#^NET&,@b.La4Q;nG`vRnvX0#v=^v%+6)`D+/XHC#3L/2'UfpxFVe##,1VNY5:R%q77AUg*XD<;&"
    "0;jt(%O2-MYEk'MSiI.MHsL5/0GY##jlRfLFV:6vmu?4v:L,W-R^9[KL_l3+G<8F._Zv%++35NLT_^]+tX<X(K%R-HpM`.L'ltA#ds3L#w:.`aQOJJr)lak4Mm.T/$_-L,,#<X(o@4ip"
    "xeB#v(hio.MXaq2*4U'#4Sl##u8q'#`WajLJIQuu(.W'Me784v)b#7vk+B;-b0#W/5RK:#gf>3#.#]&#6-c?K78[w'wr?rm=MQk=0=?*Nn(<$#[g(P-bV1]M.x<J-0aXv-:.+,MMIVcD"
    "YZR-Q0r-5/K,j3(&G_s/.@,gLvjA%#(@hhLswSfL+n<8v%?G)MH76>#sR.fMOD`2NsJH)MJdT5#JrC$#Q#M*M-Nn7v]Cg;-1ClG-?_.u-a4&cNtJd`M)5aEMCer=-JQs=-sXcG-`A37N"
    "/WZY#E[9oe^d.thx]W]+RutGMxS/O&s<w0#:IQh,.6Ke$jEvE7MK0F%1#'58sB9/Dg8W#v^31fqA_8D3&c@Mq&`bxu)VQ)4eSb'/8@'&4<rck4ETt?0*?*cr0iVu$Ib`78L5(B#TGdAM"
    "62RA-[[Ok12oNT#?FX&#N7p*#wlWrL18CP8),p*%fg[dMP4b(Oj_wb%]+>gLDlQiM>fd2&J$J;)n[?,v?2`*MAlg9vYI.+Md#TfLGnv##'2^V-mLaDuwDKW$X7RS%)Aq;-N@GdMk[d##"
    "eV@)tf#I`NJ9tZg^TH&#KQn;%D*?e62au'&_E]s--35)MZJB9vbaTqD?jwf;m>^m8^JMS]*782Lvu];%M%pxun=#s--iuaRZ./NQ8_H'v8D%+MLsJfL#JgnL(XPgL`M.Q-vLPS.V-?;#"
    "/_G-MDgI>/hGS9v%kQiLZ;-##-.m<-%I`t-SZ3ZMfF5gLHL>gLV>Mq%@'8F.2kvcM-7JqLNB=983`u)4r3'8@(Kn)>4Vpw%60WdM9pkw&jhRg<Sc_2)CHOW-XQ3L#RG###_L1m(W/Ra5"
    "/g'98,B1WfFOU`.tATS%t=58.$b)P]X?_g*DiT=##`:5/G*FxuS/`*M.0'W$r3Ap.P0Z7vJ9w0#dG+W%8AGd$2[0cV^WH&#9'sS&_A5<-h$Mq'lQI3/9P4^#J)Z`35,>>#l#jW#,Gc>#"
    "0Su>#4`1?#ugXU%^UWP&1E#m&@$TM'i'`2F,weF-PmnUCg4'oDLAEQC3A2eGisq$'`Z?qC/[u'&l8*7D:p=HMYcIdF>PZ1FETM2:XXGx5c%N]&w?vsB`lDcHD+7X&+SV.GI*c3CuV?%&"
    "):FW--JiaH]TxfCt4On).oViFFI*hFX`g<I.K1U16Y7fGE>pq)`_#MOX4$##1A8.CH67$#";

void ImGuiHelpers::Init()
{
    ImGui::GetIO().Fonts->AddFontDefault();
    static const ImWchar icons_ranges[] = {ICON_MIN_lb3k, ICON_MAX_lb3k, 0};

    // static const ImWchar icons_ranges[] = {58000, 58014, 0};

    ImFontConfig icons_config;
    icons_config.MergeMode        = true;
    icons_config.OversampleH      = 1;
    icons_config.GlyphMinAdvanceX = 8;

    

    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_lb3k, 21, &icons_config,
                                                               icons_ranges);


    g_pToolbarIconsSheet = TextureManager::Instance()->LoadTextureSynch("res/ui/common_icons.png");
}

bool ImGuiHelpers::ImageButtonWithText(ImTextureID texId, const char *label, const ImVec2 &imageSize, const ImVec2 &uv0,
                                       const ImVec2 &uv1, int frame_padding, const ImVec4 &bg_col,
                                       const ImVec4 &tint_col)
{
    using namespace ImGui;

    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImVec2 size = imageSize;
    if (size.x <= 0 && size.y <= 0)
    {
        size.x = size.y = ImGui::GetTextLineHeightWithSpacing();
    }
    else
    {
        if (size.x <= 0)
            size.x = size.y;
        else if (size.y <= 0)
            size.y = size.x;
        size.x *= window->FontWindowScale * ImGui::GetIO().FontGlobalScale;
        size.y *= window->FontWindowScale * ImGui::GetIO().FontGlobalScale;
    }

    ImGuiContext &g         = *GImGui;
    const ImGuiStyle &style = g.Style;

    const ImGuiID id      = window->GetID(label);
    const ImVec2 textSize = ImGui::CalcTextSize(label, NULL, true);
    const bool hasText    = textSize.x > 0;

    const float innerSpacing =
        hasText ? ((frame_padding >= 0) ? (float)frame_padding : (style.ItemInnerSpacing.x)) : 0.f;
    const ImVec2 padding =
        (frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
    const ImVec2 totalSizeWithoutPadding(size.x + innerSpacing + textSize.x, size.y > textSize.y ? size.y : textSize.y);
    // const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + totalSizeWithoutPadding + padding * 2);
    ImRect bb;
    bb.Min = window->DC.CursorPos;
    bb.Max = window->DC.CursorPos;
    bb.Max.x += totalSizeWithoutPadding.x + padding.x * 2;
    bb.Max.y += totalSizeWithoutPadding.y + padding.y * 2;

    ImVec2 start(0, 0);
    start = window->DC.CursorPos;
    start.x += padding.x;
    start.y += padding.y;

    if (size.y < textSize.y)
        start.y += (textSize.y - size.y) * .5f;
    ImRect image_bb;

    image_bb.Min = start;
    image_bb.Max = start;
    image_bb.Max.x += size.x;
    image_bb.Max.y += size.y;

    start = window->DC.CursorPos;
    start.x += padding.x;
    start.y += padding.y;

    start.x += size.x + innerSpacing;
    if (size.y > textSize.y)
        start.y += (size.y - textSize.y) * .5f;
    ItemSize(bb);
    if (!ItemAdd(bb, id))
        return false;

    bool hovered = false, held = false;
    bool pressed = ButtonBehavior(bb, id, &hovered, &held);

    // Render

    const ImU32 col = GetColorU32((hovered && held) ? ImGuiCol_ButtonActive
                                  : hovered         ? ImGuiCol_ButtonHovered
                                                    : ImGuiCol_Border);

    RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));

    window->DrawList->AddImage(texId, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));

    if (textSize.x > 0)
        ImGui::RenderText(start, label);
    return pressed;
}

bool ImGuiHelpers::ButtonWithCommonTextureIcon(CommonTextures icon, const char *label, const float icon_size,
                                        ImVec4 tint /*= {1,1,1,1}*/)
{
    auto icn    = GetCommonIcon(icon);
    auto style   = ImGui::GetStyle();
    auto bgColor = style.Colors[ImGuiCol_Button];

    return ImGuiHelpers::ImageButtonWithText((ImTextureID)icn->GLTextureNum(), label, ImVec2(icon_size, icon_size),
                                             ImVec2(1, 1), ImVec2(0, 0), style.FramePadding.x, bgColor, tint);
}

bool ImGuiHelpers::ButtonWithCommonIcon(CommonIcons icon, const char *label, const float icon_size,
                                     ImVec4 tint /*= {1, 1, 1, 1}*/)
{    
    auto style   = ImGui::GetStyle();
    auto bgColor = style.Colors[ImGuiCol_Button];

    ImVec4 uv;
    if (icon != CommonIcons::None)
        uv = g_CommonIconsRects[(int)icon];
    else
        uv = ImVec4(1, 1, 1, 1);

    ImVec2 uv0 = ImVec2(uv.x, 1 - uv.y);
    ImVec2 uv1 = ImVec2(uv.z, 1 - uv.w);

    ImVec2 imageSize = ImVec2(icon_size, icon_size);
    return ImGui::ImageButton(label,(ImTextureID)g_pToolbarIconsSheet->GLTextureNum(), imageSize, uv0, uv1, bgColor, tint);

    //return ImGuiHelpers::ImageButtonWithText((ImTextureID)g_pToolbarIconsSheet->GLTextureNum(), label, ImVec2(icon_size, icon_size),
      //                                       uv0, uv1, style.FramePadding.x, bgColor, tint);
}

void ImGuiHelpers::DisplayCommonIcon(CommonIcons icon, const float size)
{
    ImVec4 uv = g_CommonIconsRects[(int)icon];

    ImVec2 uv0 = ImVec2(uv.x, 1 - uv.y);
    ImVec2 uv1 = ImVec2(uv.z, 1 - uv.w);

    ImVec2 imageSize = ImVec2(size, size);
    ImGui::Image((ImTextureID)g_pToolbarIconsSheet->GLTextureNum(), imageSize, uv0, uv1);
}
