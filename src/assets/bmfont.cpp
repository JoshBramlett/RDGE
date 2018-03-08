#include <rdge/assets/bmfont.hpp>
#include <rdge/util/io/rwops_base.hpp>
#include <rdge/util/strings.hpp>
#include <rdge/internal/exception_macros.hpp>

#include <sstream>

#define TRY_PARSE_STRING(token, index, o, key)      \
    if (token.substr(0, index - 1) == #key) {       \
        o.key = token.substr(index);                \
        rdge::remove_all(o.key, '\"');              \
        continue;                                   \
    } else do {} while (0)

#define TRY_PARSE_INT(token, index, o, key)         \
    if (token.substr(0, index - 1) == #key) {       \
        o.key = std::stoi(token.substr(index));     \
        continue;                                   \
    } else do {} while (0)

#define TRY_PARSE_FLAG(token, index, o, key)        \
    if (token.substr(0, index - 1) == #key) {       \
        if (std::stoi(token.substr(index))) {       \
            o.flags |= rdge::bmfont_##o##_##key;    \
        } continue;                                 \
    } else do {} while (0)

namespace rdge {

namespace {

void
parse_line (const std::string& line, bmfont_info& info)
{
    // info face="BanglaSangamMN" size=32 bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 padding=4,4,4,4 spacing=-8,-8

    auto elements = rdge::split(line);
    for (auto& e : elements)
    {
        size_t p = e.find('=', 0);
        if (p == std::string::npos)
        {
            continue;
        }

        try {
            TRY_PARSE_STRING(e, p + 1, info, face);
            TRY_PARSE_STRING(e, p + 1, info, charset);

            TRY_PARSE_INT(e, p + 1, info, size);
            TRY_PARSE_INT(e, p + 1, info, stretchH);
            TRY_PARSE_INT(e, p + 1, info, aa);
            TRY_PARSE_INT(e, p + 1, info, outline);

            TRY_PARSE_FLAG(e, p + 1, info, bold);
            TRY_PARSE_FLAG(e, p + 1, info, italic);
            TRY_PARSE_FLAG(e, p + 1, info, unicode);
            TRY_PARSE_FLAG(e, p + 1, info, smooth);

            if (rdge::starts_with(e, "padding"))
            {
                auto paddings = rdge::split(e.substr(p + 1), ',');
                if (paddings.size() == 4)
                {
                    info.padding[0] = std::stoi(paddings[0]);
                    info.padding[1] = std::stoi(paddings[1]);
                    info.padding[2] = std::stoi(paddings[2]);
                    info.padding[3] = std::stoi(paddings[3]);
                    continue;
                }
                else
                {
                    throw std::invalid_argument("invalid padding");
                }
            }

            if (rdge::starts_with(e, "spacing"))
            {
                auto spacings = rdge::split(e.substr(p + 1), ',');
                if (spacings.size() == 2)
                {
                    info.spacing[0] = std::stoi(spacings[0]);
                    info.spacing[1] = std::stoi(spacings[1]);
                    continue;
                }
                else
                {
                    throw std::invalid_argument("invalid spacing");
                }
            }
        } catch (const std::exception& ex) {
            RDGE_THROW(ex.what());
        }
    }
}

void
parse_line (const std::string& line, bmfont_common& common)
{
    // common lineHeight=48 base=30 scaleW=512 scaleH=512 pages=1 packed=0

    auto elements = rdge::split(line);
    for (auto& e : elements)
    {
        size_t p = e.find('=', 0);
        if (p == std::string::npos)
        {
            continue;
        }

        try {
            TRY_PARSE_INT(e, p + 1, common, lineHeight);
            TRY_PARSE_INT(e, p + 1, common, base);
            TRY_PARSE_INT(e, p + 1, common, scaleW);
            TRY_PARSE_INT(e, p + 1, common, scaleH);
            TRY_PARSE_INT(e, p + 1, common, pages);
            TRY_PARSE_INT(e, p + 1, common, alphaChnl);
            TRY_PARSE_INT(e, p + 1, common, redChnl);
            TRY_PARSE_INT(e, p + 1, common, greenChnl);
            TRY_PARSE_INT(e, p + 1, common, blueChnl);

            TRY_PARSE_FLAG(e, p + 1, common, packed);
        } catch (const std::exception& ex) {
            RDGE_THROW(ex.what());
        }
    }
}

void
parse_line (const std::string& line, bmfont_page& page)
{
    // page id=0 file="banola.png"

    auto elements = rdge::split(line);
    for (auto& e : elements)
    {
        size_t p = e.find('=', 0);
        if (p == std::string::npos)
        {
            continue;
        }

        try {
            TRY_PARSE_STRING(e, p + 1, page, file);
            TRY_PARSE_INT(e, p + 1, page, id);
        } catch (const std::exception& ex) {
            RDGE_THROW(ex.what());
        }
    }
}

void
parse_line (const std::string& line, bmfont_char& c)
{
    // char id=0       x=0    y=0    width=0    height=0    xoffset=-4   yoffset=0    xadvance=0    page=0    chnl=0

    auto elements = rdge::split(line);
    for (auto& e : elements)
    {
        size_t p = e.find('=', 0);
        if (p == std::string::npos)
        {
            continue;
        }

        try {
            TRY_PARSE_INT(e, p + 1, c, id);
            TRY_PARSE_INT(e, p + 1, c, x);
            TRY_PARSE_INT(e, p + 1, c, y);
            TRY_PARSE_INT(e, p + 1, c, width);
            TRY_PARSE_INT(e, p + 1, c, height);
            TRY_PARSE_INT(e, p + 1, c, xoffset);
            TRY_PARSE_INT(e, p + 1, c, yoffset);
            TRY_PARSE_INT(e, p + 1, c, xadvance);
            TRY_PARSE_INT(e, p + 1, c, page);
            TRY_PARSE_INT(e, p + 1, c, chnl);
        } catch (const std::exception& ex) {
            RDGE_THROW(ex.what());
        }
    }
}

} // anonymous namespace

void
load_bmfont (const char* filepath, bmfont_data& font)
{
    auto config = GetTextFileContent(filepath);
    if (config.empty())
    {
        RDGE_THROW("File does not exist or is empty" + std::string(filepath));
    }

    std::istringstream input(config);
    std::string line;

    if (std::getline(input, line) && rdge::starts_with(line, "info"))
    {
        parse_line(line, font.info);
    }
    else
    {
        RDGE_THROW("File missing required \'info\' tag");
    }

    if (std::getline(input, line) && rdge::starts_with(line, "common"))
    {
        parse_line(line, font.common);
    }
    else
    {
        RDGE_THROW("File missing required \'common\' tag");
    }

    if (font.common.pages > 0)
    {
        font.pages.reserve(font.common.pages);
        for (int32 i = 0; i < font.common.pages; i++)
        {
            if (std::getline(input, line) && rdge::starts_with(line, "page"))
            {
                bmfont_page p;
                parse_line(line, p);

                font.pages.emplace_back(std::move(p));
            }
            else
            {
                RDGE_THROW("File missing required \'page\' tag");
            }
        }
    }
    else
    {
        RDGE_THROW("Missing required \'common.pages\' value");
    }

    int32 char_count = 0;
    if (std::getline(input, line) && rdge::starts_with(line, "chars"))
    {
        try {
            auto token = line.substr(line.find("count"));
            size_t p = token.find('=', 0);
            if (p != std::string::npos)
            {
                char_count = std::stoi(token.substr(p + 1));
            }
        } catch (const std::exception& ex) {
            RDGE_THROW("Unable to parse required \'chars.count\' value");
        }
    }

    if (char_count > 0)
    {
        font.pages.reserve(char_count);
        for (int32 i = 0; i < char_count; i++)
        {
            if (std::getline(input, line) && rdge::starts_with(line, "char"))
            {
                bmfont_char c;
                parse_line(line, c);

                font.high_id = std::max(font.high_id, c.id);
                font.chars.emplace_back(std::move(c));
            }
            else
            {
                RDGE_THROW("File missing required \'char\' tag");
            }
        }
    }
    else
    {
        RDGE_THROW("Missing required \'chars.count\' value");
    }
}

} // namespace rdge
