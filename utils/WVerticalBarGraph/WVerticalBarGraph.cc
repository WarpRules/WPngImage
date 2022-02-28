#include "WVerticalBarGraph.hh"
#include <cstdint>
#include <algorithm>
#include <cmath>

static int alignedX(int x, WVerticalBarGraph::LabelGeometry::HAlign hAlignment, int imageWidth)
{
    switch(hAlignment)
    {
      case WVerticalBarGraph::LabelGeometry::HAlign::center: return x - imageWidth/2;
      case WVerticalBarGraph::LabelGeometry::HAlign::right: return x - imageWidth + 1;
      default: return x;
    }
}

static int alignedY(int y, WVerticalBarGraph::LabelGeometry::VAlign vAlignment, int imageHeight)
{
    switch(vAlignment)
    {
      case WVerticalBarGraph::LabelGeometry::VAlign::center: return y - imageHeight/2;
      case WVerticalBarGraph::LabelGeometry::VAlign::bottom: return y - imageHeight + 1;
      default: return y;
    }
}

void WVerticalBarGraph::LabelGeometry::drawImage(WPngImage& destImage, int destX, int destY, const WPngImage& image)
{
    destImage.drawImage(alignedX(destX, horizontalAlignment, image.width()) + xOffset,
                        alignedY(destY, verticalAlignment, image.height()) + yOffset, image);
}

void WVerticalBarGraph::LabelGeometry::drawImage(WPngImage& destImage, int destX, int destY, const WPngImage& image,
                                                 int srcX, int srcY, int srcWidth, int srcHeight)
{
    destImage.drawImage(alignedX(destX, horizontalAlignment, srcWidth) + xOffset,
                        alignedY(destY, verticalAlignment, srcHeight) + yOffset, image,
                        srcX, srcY, srcWidth, srcHeight);
}

static int scaleValue(int value, int factor1, int factor2, bool roundToNearest)
{
    std::int64_t value64 = value;
    value64 *= static_cast<std::int64_t>(factor1) * static_cast<std::int64_t>(factor2);
    if(roundToNearest) value64 = (value64 >= 0 ? value64 + 500000 : value64 - 500000);
    return static_cast<int>(value64 / 1000000);
}

static int multiplyFactors(int factor1, int factor2, int factor3, int factor4, bool roundToNearest)
{
    std::int64_t factor1_64 = factor1, factor2_64 = factor2, factor3_64 = factor3, factor4_64 = factor4;;

    if(!roundToNearest)
        return static_cast<int>((((factor1_64 * factor2_64 * factor3_64) / 1000 * factor4_64) / 1000000000));

    factor1_64 *= factor2_64 * factor3_64;
    factor1_64 = (factor1_64 >= 0 ? factor1_64 + 500 : factor1_64 - 500) / 1000;
    factor1_64 *= factor4_64;
    return static_cast<int>((factor1_64 >= 0 ? factor1_64 + 500000000 : factor1_64 - 500000000) / 1000000000);
}

static int scalePosition(int position, const WVerticalBarGraph::Settings& settings)
{
    return scaleValue(position, settings.scalingFactorPerMille_Positions,
                      settings.scalingFactorPerMille_Global, settings.roundScaledValuesToNearest);
}

static int scaleWidth(int width, const WVerticalBarGraph::Settings& settings)
{
    return scaleValue(width, settings.scalingFactorPerMille_LineWidths,
                      settings.scalingFactorPerMille_Global, settings.roundScaledValuesToNearest);
}

static int scaleBarHeight(int heightPermille, const WVerticalBarGraph::Settings& settings)
{
    return multiplyFactors(heightPermille, settings.scalingFactorPerMille_BarHeights, settings.scalingFactorPerMille_Positions,
                           settings.scalingFactorPerMille_Global, settings.roundScaledValuesToNearest);
}

static void scaleSettings(WVerticalBarGraph::Settings& settings)
{
    settings.scalingFactorPerMille_BarHeights *= settings.bar_TotalWidth;
    settings.imageMargin_Left = scalePosition(settings.imageMargin_Left, settings);
    settings.imageMargin_Right = scalePosition(settings.imageMargin_Right, settings);
    settings.imageMargin_Bottom = scalePosition(settings.imageMargin_Bottom, settings);
    settings.imageMargin_Top = scalePosition(settings.imageMargin_Top, settings);
    settings.xAxis_Width = scaleWidth(settings.xAxis_Width, settings);
    settings.xAxis_LengthAfterLastBar = scalePosition(settings.xAxis_LengthAfterLastBar, settings);
    settings.yAxis_Width = scaleWidth(settings.yAxis_Width, settings);
    settings.yAxis_LengthAfterHighestBar = scalePosition(settings.yAxis_LengthAfterHighestBar, settings);
    settings.yAxis_Tick_Length = scalePosition(settings.yAxis_Tick_Length, settings);
    settings.yAxis_Tick_Width = scaleWidth(settings.yAxis_Tick_Width, settings);
    settings.yAxis_Tick_GridLine_Width = scaleWidth(settings.yAxis_Tick_GridLine_Width, settings);
    settings.yAxis_Tick_GridLine_LengthAfterLastBar = scalePosition(settings.yAxis_Tick_GridLine_LengthAfterLastBar, settings);
    settings.yAxis_Tick_Label.xOffset = scalePosition(settings.yAxis_Tick_Label.xOffset, settings);
    settings.yAxis_Tick_Label.yOffset = scalePosition(settings.yAxis_Tick_Label.yOffset, settings);
    settings.yAxis_ValueMarkerLine_Width = scaleWidth(settings.yAxis_ValueMarkerLine_Width, settings);
    settings.yAxis_ValueMarkerLine_LengthAfterLastBar = scalePosition(settings.yAxis_ValueMarkerLine_LengthAfterLastBar, settings);
    settings.yAxis_ValueMarker_Label.xOffset = scalePosition(settings.yAxis_ValueMarker_Label.xOffset, settings);
    settings.yAxis_ValueMarker_Label.yOffset = scalePosition(settings.yAxis_ValueMarker_Label.yOffset, settings);
    //settings.bar_TotalWidth = scalePosition(settings.bar_TotalWidth, settings); // This cannot be scaled here
    settings.bar_BorderWidth = scaleWidth(settings.bar_BorderWidth, settings);
    //settings.bar_Spacing = scalePosition(settings.bar_Spacing, settings); // This cannot be scaled here
    settings.bar_firstBarSpacingFromYAxis = scalePosition(settings.bar_firstBarSpacingFromYAxis, settings);
    settings.bar_Label.xOffset = scalePosition(settings.bar_Label.xOffset, settings);
    settings.bar_Label.yOffset = scalePosition(settings.bar_Label.yOffset, settings);
}

namespace
{
    struct GraphGeometry
    {
        int graphWidth, graphHeight;
        int graphOriginY;
        int minBarHeight, maxBarHeight;
    };
}

static void draw_YAxis_GridLines(WPngImage& destImage, const WVerticalBarGraph::GraphData&,
                                 const WVerticalBarGraph::Settings& settings, const GraphGeometry& graphGeometry)
{
    if(settings.yAxis_Tick_GridLine_Width <= 0) return;

    const int startX = settings.imageMargin_Left - settings.yAxis_Width / 2 + settings.yAxis_Width;
    const int endX = settings.imageMargin_Left + graphGeometry.graphWidth;
    const int lineLength = endX - startX;
    const int yOffset = settings.yAxis_Tick_GridLine_Width / 2;

    for(int unscaledYOffset = settings.yAxis_Tick_SteppingPermille; true; unscaledYOffset += settings.yAxis_Tick_SteppingPermille)
    {
        const int y = graphGeometry.graphOriginY - scaleBarHeight(unscaledYOffset, settings) + yOffset;
        const int lineTopEdgeY = y - settings.yAxis_Tick_GridLine_Width + 1;
        if(lineTopEdgeY < settings.imageMargin_Top) break;
        destImage.drawRect(startX, y, lineLength, -settings.yAxis_Tick_GridLine_Width, settings.color_YAxis_GridLine, true);
    }

    if(graphGeometry.minBarHeight == 0) return;

    const int maxY = destImage.height() - settings.imageMargin_Bottom;
    for(int unscaledYOffset = settings.yAxis_Tick_SteppingPermille; true; unscaledYOffset += settings.yAxis_Tick_SteppingPermille)
    {
        const int y = graphGeometry.graphOriginY + scaleBarHeight(unscaledYOffset, settings) + yOffset;
        if(y >= maxY) break;
        destImage.drawRect(startX, y, lineLength, -settings.yAxis_Tick_GridLine_Width, settings.color_YAxis_GridLine, true);
    }
}

static void draw_YAxis_Ticks(WPngImage& destImage, const WVerticalBarGraph::GraphData&,
                             const WVerticalBarGraph::Settings& settings, const GraphGeometry& graphGeometry)
{
    if(settings.yAxis_Tick_Length == 0 || settings.yAxis_Tick_Width <= 0) return;

    const int startX = settings.imageMargin_Left - settings.yAxis_Width / 2 - 1;
    const int tickBottomEdgeYOffset = settings.yAxis_Tick_Width / 2;

    for(int unscaledYOffset = settings.yAxis_Tick_DrawTickAtOrigin ? 0 : settings.yAxis_Tick_SteppingPermille; true;
        unscaledYOffset += settings.yAxis_Tick_SteppingPermille)
    {
        const int y = graphGeometry.graphOriginY - scaleBarHeight(unscaledYOffset, settings) + tickBottomEdgeYOffset;
        const int lineTopEdgeY = y - settings.yAxis_Tick_GridLine_Width + 1;
        if(lineTopEdgeY < settings.imageMargin_Top) break;
        destImage.drawRect(startX, y, -settings.yAxis_Tick_Length, -settings.yAxis_Tick_Width, settings.color_YAxis_Tick, true);
    }

    if(graphGeometry.minBarHeight == 0) return;

    const int maxY = destImage.height() - settings.imageMargin_Bottom;
    for(int unscaledYOffset = settings.yAxis_Tick_SteppingPermille; true; unscaledYOffset += settings.yAxis_Tick_SteppingPermille)
    {
        const int y = graphGeometry.graphOriginY + scaleBarHeight(unscaledYOffset, settings) + tickBottomEdgeYOffset;
        if(y >= maxY) break;
        destImage.drawRect(startX, y, -settings.yAxis_Tick_Length, -settings.yAxis_Tick_Width, settings.color_YAxis_Tick, true);
    }
}

static void draw_Bars(WPngImage& destImage, const WVerticalBarGraph::GraphData& graphData,
                      const WVerticalBarGraph::Settings& settings, const GraphGeometry& graphGeometry)
{
    if(settings.bar_TotalWidth <= 0) return;

    const int startX = settings.imageMargin_Left + settings.bar_firstBarSpacingFromYAxis;
    const int unscaledXStep = settings.bar_TotalWidth + settings.bar_Spacing;
    const int barTotalWidth = scalePosition(settings.bar_TotalWidth, settings);
    const int barBorderWidth2 = settings.bar_BorderWidth * 2;
    const int xAxis_Width = settings.xAxis_Width < 0 ? 0 : settings.xAxis_Width;
    const int xAxisTopsideY = graphGeometry.graphOriginY + xAxis_Width/2 - settings.xAxis_Width;
    const int xAxisUndersideY = graphGeometry.graphOriginY + xAxis_Width/2 + 1;

    int unscaledBarX = 0;
    for(std::size_t barIndex = 0; barIndex < graphData.barHeightsAmount; ++barIndex, unscaledBarX += unscaledXStep)
    {
        const int barX = startX + scalePosition(unscaledBarX, settings);
        const int barHeight = scaleBarHeight(graphData.barHeightPermilles[barIndex], settings);
        const int barEndY = graphGeometry.graphOriginY - barHeight + (barHeight > 0 ? -1 : 1);
        const int barStartY = barHeight > 0 ? xAxisTopsideY : xAxisUndersideY;
        const int rectHeight = barEndY - barStartY;

        if(settings.bar_BorderWidth <= 0)
        {
            destImage.drawRect(barX, barStartY, barTotalWidth, rectHeight, settings.color_Bar_Inside, true);
        }
        else
        {
            int barInsideStartY, barInsideEndY;
            if(barHeight > 0)
            {
                barInsideStartY = barStartY - (settings.bar_DrawBottomBorder ? settings.bar_BorderWidth : 0);
                barInsideEndY = barEndY + settings.bar_BorderWidth;
            }
            else
            {
                barInsideStartY = barStartY + (settings.bar_DrawBottomBorder ? settings.bar_BorderWidth : 0);
                barInsideEndY = barEndY - settings.bar_BorderWidth;
            }
            const int barInsideRectHeight = barInsideEndY - barInsideStartY;

            if(barBorderWidth2 >= barTotalWidth || (barHeight >= 0) != (barInsideRectHeight < 0))
            {
                destImage.drawRect(barX, barStartY, barTotalWidth, rectHeight, settings.color_Bar_Border, true);
            }
            else
            {
                const int barInsideX = barX + settings.bar_BorderWidth;
                const int barInsideWidth = barTotalWidth - barBorderWidth2;
                destImage.drawRect(barInsideX, barInsideStartY, barInsideWidth, barInsideRectHeight, settings.color_Bar_Inside, true);

                destImage.drawRect(barX, barStartY, settings.bar_BorderWidth, rectHeight, settings.color_Bar_Border, true);
                destImage.drawRect(barX + barTotalWidth - settings.bar_BorderWidth, barStartY,
                                   settings.bar_BorderWidth, rectHeight, settings.color_Bar_Border, true);
                destImage.drawRect(barInsideX, barInsideEndY, barInsideWidth, barEndY - barInsideEndY, settings.color_Bar_Border, true);

                if(settings.bar_DrawBottomBorder)
                {
                    const int bottomBorderEndY = barStartY + (barHeight > 0 ? -settings.bar_BorderWidth : settings.bar_BorderWidth);
                    destImage.drawRect(barInsideX, barStartY,
                                       barInsideWidth, bottomBorderEndY - barStartY, settings.color_Bar_Border, true);
                }
            }
        }
    }
}

static void draw_Axes(WPngImage& destImage, const WVerticalBarGraph::GraphData& ,
                      const WVerticalBarGraph::Settings& settings, const GraphGeometry& graphGeometry)
{
    const int xAxis_Width = settings.xAxis_Width < 0 ? 0 : settings.xAxis_Width;
    const int yAxis_Width = settings.yAxis_Width < 0 ? 0 : settings.yAxis_Width;
    const int yAxisX = settings.imageMargin_Left - yAxis_Width / 2;
    const int xAxisY = graphGeometry.graphOriginY + xAxis_Width / 2;

    if(xAxis_Width > 0)
        destImage.drawRect(yAxisX, xAxisY, graphGeometry.graphWidth + yAxis_Width/2, -xAxis_Width, settings.color_XAxis, true);

    if(yAxis_Width > 0)
    {
        if(graphGeometry.maxBarHeight > 0)
            destImage.drawRect(yAxisX, xAxisY - xAxis_Width, yAxis_Width, -(xAxisY - settings.imageMargin_Top - xAxis_Width + 1),
                               settings.color_YAxis, true);

        if(graphGeometry.minBarHeight < 0)
            destImage.drawRect(yAxisX, xAxisY + 1, yAxis_Width, destImage.height() - settings.imageMargin_Bottom - xAxisY - 1,
                               settings.color_YAxis, true);
    }
}

static void draw_YAxis_ValueMarkerLines(WPngImage& destImage, const WVerticalBarGraph::GraphData& graphData,
                                        const WVerticalBarGraph::Settings& settings, const GraphGeometry& graphGeometry)
{
    if(settings.yAxis_ValueMarkerLine_Width <= 0 || graphData.yAxisValuesAmount <= 0) return;

    const int startX = settings.imageMargin_Left - settings.yAxis_Width / 2 + settings.yAxis_Width;
    const int endX = settings.imageMargin_Left + graphGeometry.graphWidth;
    const int lineLength = endX - startX;
    const int yOffset = settings.yAxis_ValueMarkerLine_Width / 2;

    for(std::size_t i = 0; i < graphData.yAxisValuesAmount; ++i)
    {
        const int y = graphGeometry.graphOriginY - scaleBarHeight(graphData.yAxisValuePermilles[i], settings) + yOffset;
        destImage.drawRect(startX, y, lineLength, -settings.yAxis_ValueMarkerLine_Width, settings.color_YAxis_ValueMarkerLine, true);
    }
}

static void draw_YAxis_TickLabels(WPngImage& destImage, const WVerticalBarGraph::GraphData& graphData,
                                  const WVerticalBarGraph::Settings& settings, const GraphGeometry& graphGeometry)
{
    if(!graphData.drawLabelFunc_yAxisTick) return;

    const int tickBottomEdgeYOffset = settings.yAxis_Tick_Width / 2;
    int tickIndex = settings.yAxis_Tick_DrawTickAtOrigin;
    int unscaledYOffset = settings.yAxis_Tick_DrawTickAtOrigin ? 0 : settings.yAxis_Tick_SteppingPermille;

    for(; true; ++tickIndex, unscaledYOffset += settings.yAxis_Tick_SteppingPermille)
    {
        const int y = graphGeometry.graphOriginY - scaleBarHeight(unscaledYOffset, settings);
        const int lineTopEdgeY = y + tickBottomEdgeYOffset - settings.yAxis_Tick_GridLine_Width + 1;
        if(lineTopEdgeY < settings.imageMargin_Top) break;
        graphData.drawLabelFunc_yAxisTick(tickIndex, destImage, settings.imageMargin_Left, y, settings.yAxis_Tick_Label);
    }

    if(graphGeometry.minBarHeight == 0) return;

    tickIndex = -1;
    unscaledYOffset = settings.yAxis_Tick_SteppingPermille;
    const int maxY = destImage.height() - settings.imageMargin_Bottom;
    for(; true; unscaledYOffset += settings.yAxis_Tick_SteppingPermille)
    {
        const int y = graphGeometry.graphOriginY + scaleBarHeight(unscaledYOffset, settings);
        if(y + tickBottomEdgeYOffset >= maxY) break;
        graphData.drawLabelFunc_yAxisTick(tickIndex, destImage, settings.imageMargin_Left, y, settings.yAxis_Tick_Label);
    }
}

static void draw_BarLabels(WPngImage& destImage, const WVerticalBarGraph::GraphData& graphData,
                           const WVerticalBarGraph::Settings& settings, const GraphGeometry& graphGeometry)
{
    if(!graphData.drawLabelFunc_Bar) return;

    const int barTotalWidth = scalePosition(settings.bar_TotalWidth, settings);
    const int startX = settings.imageMargin_Left + settings.bar_firstBarSpacingFromYAxis + barTotalWidth / 2;
    const int unscaledXStep = settings.bar_TotalWidth + settings.bar_Spacing;

    int unscaledBarX = 0;
    for(std::size_t barIndex = 0; barIndex < graphData.barHeightsAmount; ++barIndex, unscaledBarX += unscaledXStep)
    {
        const int barX = startX + scalePosition(unscaledBarX, settings);
        WVerticalBarGraph::LabelGeometry geometry = settings.bar_Label;
        if((graphData.barHeightPermilles[barIndex] < 0) != (!settings.bar_Label_BelowBar))
        {
            if(geometry.verticalAlignment == WVerticalBarGraph::LabelGeometry::VAlign::bottom)
                geometry.verticalAlignment = WVerticalBarGraph::LabelGeometry::VAlign::top;
            else if(geometry.verticalAlignment == WVerticalBarGraph::LabelGeometry::VAlign::top)
                geometry.verticalAlignment = WVerticalBarGraph::LabelGeometry::VAlign::bottom;
            geometry.yOffset = -geometry.yOffset;
        }

        if(settings.bar_Label_BelowBar)
        {
            graphData.drawLabelFunc_Bar(barIndex, destImage, barX, graphGeometry.graphOriginY, geometry);
        }
        else
        {
            const int barHeight = scaleBarHeight(graphData.barHeightPermilles[barIndex], settings);
            graphData.drawLabelFunc_Bar(barIndex, destImage, barX, graphGeometry.graphOriginY - barHeight, geometry);
        }
    }
}

static void draw_YAxis_ValueMarkerLabels(WPngImage& destImage, const WVerticalBarGraph::GraphData& graphData,
                                         const WVerticalBarGraph::Settings& settings, const GraphGeometry& graphGeometry)
{
    if(!graphData.drawLabelFunc_yAxisValueMarker || graphData.yAxisValuesAmount <= 0) return;

    for(std::size_t i = 0; i < graphData.yAxisValuesAmount; ++i)
    {
        const int y = graphGeometry.graphOriginY - scaleBarHeight(graphData.yAxisValuePermilles[i], settings);
        graphData.drawLabelFunc_yAxisValueMarker(static_cast<int>(i), destImage, settings.imageMargin_Left, y, settings.yAxis_ValueMarker_Label);
    }
}

using DrawBarGraphElementFuncPtr = void(*)(WPngImage&, const WVerticalBarGraph::GraphData&,
                                           const WVerticalBarGraph::Settings&, const GraphGeometry&);

namespace
{
    struct ElementDrawFuncData
    {
        int zDepth;
        DrawBarGraphElementFuncPtr drawFunc;
        bool operator<(const ElementDrawFuncData& rhs) const { return zDepth < rhs.zDepth; }
    };
}

static void drawBarGraph(WPngImage& destImage, const WVerticalBarGraph::GraphData& graphData,
                         const WVerticalBarGraph::Settings& settings, const GraphGeometry& graphGeometry)
{
    ElementDrawFuncData drawElementFuncs[] =
    {
        { settings.zDepth_YAxis_GridLine, draw_YAxis_GridLines },
        { settings.zDepth_YAxis_Tick, draw_YAxis_Ticks },
        { settings.zDepth_Bars, draw_Bars },
        { settings.zDepth_Axes, draw_Axes },
        { settings.zDepth_YAxis_ValueMarkerLine, draw_YAxis_ValueMarkerLines },
        { settings.zDepth_YAxis_TickLabel, draw_YAxis_TickLabels },
        { settings.zDepth_BarLabel, draw_BarLabels },
        { settings.zDepth_YAxis_ValueMarkerLabel, draw_YAxis_ValueMarkerLabels }
    };

    std::sort(drawElementFuncs, drawElementFuncs+sizeof(drawElementFuncs)/sizeof(*drawElementFuncs));
    for(const ElementDrawFuncData& data: drawElementFuncs)
        data.drawFunc(destImage, graphData, settings, graphGeometry);
}

void WVerticalBarGraph::createImage(WPngImage& destImage, const WVerticalBarGraph::GraphData& graphData,
                                    WVerticalBarGraph::Settings settings)
{
    if(!graphData.barHeightPermilles || graphData.barHeightsAmount == 0)
        return;

    scaleSettings(settings);

    GraphGeometry geometry = {};
    for(std::size_t i = 0; i < graphData.barHeightsAmount; ++i)
    {
        const int height = scaleBarHeight(graphData.barHeightPermilles[i], settings);
        if(height > geometry.maxBarHeight) geometry.maxBarHeight = height;
        else if(height < geometry.minBarHeight) geometry.minBarHeight = height;
    }

    for(std::size_t i = 0; i < graphData.yAxisValuesAmount; ++i)
    {
        const int y = scaleBarHeight(graphData.yAxisValuePermilles[i], settings);
        if(y > geometry.maxBarHeight) geometry.maxBarHeight = y;
        else if(y < geometry.minBarHeight) geometry.minBarHeight = y;
    }

    const bool positiveBars = (geometry.maxBarHeight > 0);
    const bool negativeBars = (geometry.minBarHeight < 0);

    const int unscaledBarsTotalWidth = graphData.barHeightsAmount * settings.bar_TotalWidth +
        (graphData.barHeightsAmount - 1) * settings.bar_Spacing;
    const int barsTotalWidth = scalePosition(unscaledBarsTotalWidth, settings);

    geometry.graphWidth = barsTotalWidth + settings.bar_firstBarSpacingFromYAxis + settings.xAxis_LengthAfterLastBar;

    geometry.graphHeight = geometry.maxBarHeight - geometry.minBarHeight + settings.yAxis_LengthAfterHighestBar;
    if(positiveBars && negativeBars) geometry.graphHeight += settings.yAxis_LengthAfterHighestBar;

    int imageWidth = geometry.graphWidth + settings.imageMargin_Left + settings.imageMargin_Right;
    int imageHeight = geometry.graphHeight + settings.imageMargin_Bottom + settings.imageMargin_Top;

    geometry.graphOriginY = imageHeight - 1 - settings.imageMargin_Bottom + geometry.minBarHeight;
    if(negativeBars) geometry.graphOriginY -= settings.yAxis_LengthAfterHighestBar;

    if(imageWidth < 0) imageWidth = 0;
    else if(imageWidth > settings.maximumImageWidth) imageWidth = settings.maximumImageWidth;
    if(imageHeight < 0) imageHeight = 0;
    else if(imageHeight > settings.maximumImageHeight) imageHeight = settings.maximumImageHeight;

    destImage.newImage(imageWidth, imageHeight, settings.color_Background);
    drawBarGraph(destImage, graphData, settings, geometry);
}
