#include <gtest/gtest.h>

#include <rdge/assets/file_formats/bmfont.hpp>

namespace {

using namespace rdge;

// TODO Add failure/edge case testing

TEST(BMFontTest, ValidateParsing)
{
    bmfont_data font;
    load_bmfont("../tests/testdata/assets/bitmap_font.fnt", font);

    // info face="BanglaSangamMN" size=32 bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 padding=4,4,4,4 spacing=-8,-8

    EXPECT_EQ(font.info.face, "BanglaSangamMN");
    EXPECT_EQ(font.info.size, 32);
    EXPECT_EQ(font.info.charset, "");
    EXPECT_EQ(font.info.stretchH, 100);
    EXPECT_EQ(font.info.aa, 1);
    EXPECT_EQ(font.info.flags, bmfont_info_smooth);

    EXPECT_EQ(font.info.padding[0], 4);
    EXPECT_EQ(font.info.padding[1], 4);
    EXPECT_EQ(font.info.padding[2], 4);
    EXPECT_EQ(font.info.padding[3], 4);

    EXPECT_EQ(font.info.spacing[0], -8);
    EXPECT_EQ(font.info.spacing[1], -8);

    // common lineHeight=48 base=30 scaleW=512 scaleH=512 pages=1 packed=0

    EXPECT_EQ(font.common.lineHeight, 48);
    EXPECT_EQ(font.common.base, 30);
    EXPECT_EQ(font.common.scaleW, 512);
    EXPECT_EQ(font.common.scaleH, 512);
    EXPECT_EQ(font.common.pages, 1);
    EXPECT_EQ(font.common.flags, 0);

    // page id=0 file="banola.png"

    EXPECT_EQ(font.pages.size(), 1);
    EXPECT_EQ(font.pages[0].file, "banola.png");
    EXPECT_EQ(font.pages[0].id, 0);

    // char id=126     x=109  y=145  width=29   height=17   xoffset=-3   yoffset=10   xadvance=23   page=0    chnl=0

    EXPECT_EQ(font.chars.size(), 97);
    EXPECT_EQ(font.chars[96].id, 126);
    EXPECT_EQ(font.chars[96].x, 109);
    EXPECT_EQ(font.chars[96].y, 145);
    EXPECT_EQ(font.chars[96].width, 29);
    EXPECT_EQ(font.chars[96].height, 17);
    EXPECT_EQ(font.chars[96].xoffset, -3);
    EXPECT_EQ(font.chars[96].yoffset, 10);
    EXPECT_EQ(font.chars[96].xadvance, 23);
    EXPECT_EQ(font.chars[96].page, 0);
    EXPECT_EQ(font.chars[96].chnl, 0);

    EXPECT_EQ(font.kerning_table.size(), 0);

    EXPECT_EQ(font.high_id, 126);
}

} // anonymous namespace
