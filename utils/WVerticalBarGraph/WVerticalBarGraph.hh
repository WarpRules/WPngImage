/* WVerticalBarGraph
   -----------------
   This source code is released under the MIT license.
   For full documentation, consult the WVerticalBarGraph.html file.
*/

#ifndef WVERTICALBARGRAPH_INCLUDE_GUARD
#define WVERTICALBARGRAPH_INCLUDE_GUARD
#include "../../WPngImage.hh"
#include <cstddef>
#include <functional>

namespace WVerticalBarGraph
{
    struct Settings;
    struct GraphData;
    struct LabelGeometry;

    using DrawLabelImageCallbackFunction = std::function<void(int, WPngImage&, int, int, WVerticalBarGraph::LabelGeometry)>;

    void createImage(WPngImage&, const WVerticalBarGraph::GraphData&, WVerticalBarGraph::Settings);
}

struct WVerticalBarGraph::LabelGeometry
{
    enum class HAlign { left, center, right };
    enum class VAlign { bottom, center, top };

    HAlign horizontalAlignment = HAlign::center;
    VAlign verticalAlignment = VAlign::center;
    int xOffset = 0, yOffset = 0;

    void drawImage(WPngImage& destImage, int destX, int destY, const WPngImage&);
    void drawImage(WPngImage& destImage, int destX, int destY, const WPngImage&,
                   int srcX, int srcY, int srcWidth, int srcHeight);
};

struct WVerticalBarGraph::Settings
{
    int maximumImageWidth = 65536;
    int maximumImageHeight = 65536;

    int scalingFactorPerMille_Global = 1000;
    int scalingFactorPerMille_Positions = 1000;
    int scalingFactorPerMille_LineWidths = 1000;
    int scalingFactorPerMille_BarHeights = 1000;
    bool roundScaledValuesToNearest = true;

    int imageMargin_Left = 20;
    int imageMargin_Right = 20;
    int imageMargin_Bottom = 20;
    int imageMargin_Top = 20;

    int xAxis_Width = 3;
    int xAxis_LengthAfterLastBar = 8;

    int yAxis_Width = 3;
    int yAxis_LengthAfterHighestBar = 8;
    int yAxis_Tick_Length = 0;
    int yAxis_Tick_Width = 3;
    int yAxis_Tick_SteppingPermille = 1000;
    bool yAxis_Tick_DrawTickAtOrigin = false;
    int yAxis_Tick_GridLine_Width = 0;
    int yAxis_Tick_GridLine_LengthAfterLastBar = 8;
    LabelGeometry yAxis_Tick_Label = { LabelGeometry::HAlign::right, LabelGeometry::VAlign::center, -8, 0 };
    int yAxis_ValueMarkerLine_Width = 1;
    int yAxis_ValueMarkerLine_LengthAfterLastBar = 8;
    LabelGeometry yAxis_ValueMarker_Label = { LabelGeometry::HAlign::right, LabelGeometry::VAlign::center, -8, 0 };

    int bar_TotalWidth = 25;
    int bar_BorderWidth = 2;
    bool bar_DrawBottomBorder = false;
    int bar_Spacing = 8;
    int bar_firstBarSpacingFromYAxis = 8;
    LabelGeometry bar_Label = { LabelGeometry::HAlign::center, LabelGeometry::VAlign::top, 0, 4 };
    bool bar_Label_BelowBar = true;

    WPngImage::Pixel8 color_Background { 255, 255, 255 };
    WPngImage::Pixel8 color_XAxis { 0, 0, 0 };
    WPngImage::Pixel8 color_YAxis { 0, 0, 0 };
    WPngImage::Pixel8 color_YAxis_Tick { 64, 64, 64 };
    WPngImage::Pixel8 color_YAxis_GridLine { 128, 128, 128 };
    WPngImage::Pixel8 color_YAxis_ValueMarkerLine { 240, 128, 4 };
    WPngImage::Pixel8 color_Bar_Inside { 128, 255, 128 };
    WPngImage::Pixel8 color_Bar_Border { 0, 0, 0 };

    int zDepth_YAxis_GridLine = 0;
    int zDepth_YAxis_Tick = 10;
    int zDepth_Bars = 20;
    int zDepth_Axes = 30;
    int zDepth_YAxis_ValueMarkerLine = 40;
    int zDepth_YAxis_TickLabel = 50;
    int zDepth_BarLabel = 60;
    int zDepth_YAxis_ValueMarkerLabel = 70;
};

struct WVerticalBarGraph::GraphData
{
    const int *barHeightPermilles = nullptr;
    std::size_t barHeightsAmount = 0;
    const int *yAxisValuePermilles = nullptr;
    std::size_t yAxisValuesAmount = 0;

    WVerticalBarGraph::DrawLabelImageCallbackFunction drawLabelFunc_Bar;
    WVerticalBarGraph::DrawLabelImageCallbackFunction drawLabelFunc_yAxisTick;
    WVerticalBarGraph::DrawLabelImageCallbackFunction drawLabelFunc_yAxisValueMarker;
};
#endif
