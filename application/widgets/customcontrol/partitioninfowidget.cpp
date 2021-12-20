/**
 * @copyright 2020-2021 Uniontech Technology Co., Ltd.
 *
 * @file partitioninfowidget.cpp
 *
 * @brief 磁盘信息中间部分实现类
 *
 * @date 2021-12-14 14:03
 *
 * Author: yuandandan  <yuandandan@uniontech.com>
 *
 * Maintainer: yuandandan  <yuandandan@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "partitioninfowidget.h"

#include <DFontSizeManager>
#include <QMouseEvent>

#include <QApplication>

PartitionInfoWidget::PartitionInfoWidget(QWidget *parent) : QWidget(parent)
{
    QColor color("#70BEFF");    //浅蓝色
    QColor color1("#4293FF");   //蓝色
    QColor color2("#6751E4");   //紫色
    QColor color3("#FA7104");   //橙色
    QColor color4("#FFD027");   //黄色
    QColor color5("#2CCBBE");   //绿色
    QColor color6("#2C58CB");   //深蓝色
    QColor color7("#D468F0");   //浅紫色
    QColor color8("#FA7BA4");   //粉色
    QColor color9("#BFDC58");   //浅绿色
    m_colorInfo = QVector<QColor> {color, color1, color2, color3, color4, color5, color6, color7, color8, color9};

    setMouseTracking(true);
}

void PartitionInfoWidget::setData(DeviceInfo info)
{
    m_sizeInfo.clear();
    m_pathInfo.clear();
    for (int i = 0; i < info.m_partition.size(); i++) {
        PartitionInfo partitionInfo = info.m_partition.at(i);
        double partitionSize = Utils::sectorToUnit(partitionInfo.m_sectorEnd - partitionInfo.m_sectorStart + 1, partitionInfo.m_sectorSize, SIZE_UNIT::UNIT_GIB);
        m_sizeInfo.append(partitionSize);
        m_pathInfo.append(partitionInfo.m_path);
    }

    m_totalSize = Utils::sectorToUnit(info.m_length, info.m_sectorSize, SIZE_UNIT::UNIT_GIB);

    update();
}

void PartitionInfoWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect;
    QVector<QPainterPath> path;
    for (int i = 0; i < m_sizeInfo.size(); i++) {
        QPainterPath painterPath;
        path.append(painterPath);
    }

    DPalette palette;
    QColor color;
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    if (themeType == DGuiApplicationHelper::LightType) {
        color = palette.color(DPalette::Normal, DPalette::ToolTipText);
        color.setAlphaF(0.1);
    } else if (themeType == DGuiApplicationHelper::DarkType) {
        color = palette.color(DPalette::Normal, DPalette::BrightText);
        color.setAlphaF(0.2);
    }

    rect.setX(x());
    rect.setY(y());
    rect.setWidth(width());
    rect.setHeight(height());
    QRect paintRect = QRect(0, 60, rect.width() + 1, rect.height() - 170);

    m_reectInfo.clear();
    int radius = 8;
    if (1 == m_sizeInfo.size()) {
        path[0].moveTo(paintRect.bottomRight() - QPoint(0, radius));
        path[0].lineTo(paintRect.topRight() + QPoint(0, radius));
        path[0].arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                            QSize(radius * 2, radius * 2)), 0, 90);
        path[0].lineTo(paintRect.topLeft() + QPoint(radius, 0));
        path[0].arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
        path[0].lineTo(paintRect.bottomLeft() - QPoint(0, radius));
        path[0].arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                            QSize(radius * 2, radius * 2)), 180, 90);
        path[0].lineTo(paintRect.bottomRight() - QPoint(radius, 0));
        path[0].arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                            QSize(radius * 2, radius * 2)), 270, 90);

        if (m_pathInfo.at(0) == "unallocated") {
            painter.setBrush(QBrush(color));
            painter.setPen(QPen(color, 3));
            painter.fillPath(path[0], color);
        } else {
            painter.setBrush(QBrush(m_colorInfo[0]));
            painter.setPen(QPen(QColor(m_colorInfo[0]), 3));
            painter.fillPath(path[0], m_colorInfo[0]);
        }

        m_reectInfo.append(path[0].controlPointRect());
    } else {
        //根据color和size数据遍历绘制矩形
        for (int i = 0; i < m_sizeInfo.size(); i++) {
            if (i == 0) {
                path[0].moveTo(paintRect.topLeft() + QPoint(radius, 0));
                path[0].arcTo(QRect(QPoint(paintRect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
                path[0].lineTo(paintRect.bottomLeft() - QPoint(0, radius));
                path[0].arcTo(QRect(QPoint(paintRect.bottomLeft() - QPoint(0, radius * 2)),
                                    QSize(radius * 2, radius * 2)), 180, 90);
                path[0].lineTo(paintRect.bottomLeft() + QPoint(static_cast<int>((m_sizeInfo[0] / m_totalSize) * paintRect.width() + radius), 0));
                path[0].lineTo(paintRect.topLeft() + QPoint(static_cast<int>((m_sizeInfo[0] / m_totalSize) * paintRect.width() + radius), 0));
                path[0].lineTo(paintRect.topLeft() + QPoint(radius, 0));
                if (m_pathInfo.at(0) == "unallocated") {
                    painter.setBrush(QBrush(color));
                    painter.setPen(QPen(color, 3));
                    painter.fillPath(path[0], color);
                } else {
                    painter.setBrush(QBrush(m_colorInfo[0]));
                    painter.setPen(QPen(QColor(m_colorInfo[0]), 3));
                    painter.fillPath(path[0], m_colorInfo[0]);
                }

                m_reectInfo.append(path[0].controlPointRect());
            } else if (i > 0 && i < m_sizeInfo.size() - 1) {
                path[i].moveTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width()), 0));
                path[i].lineTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width() + (m_sizeInfo[i] / m_totalSize) * paintRect.width()), 0));
                path[i].lineTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width() + (m_sizeInfo[i] / m_totalSize) * paintRect.width()), paintRect.height() - 1));
                path[i].lineTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width()), paintRect.height() - 1));
                path[i].lineTo(path[i - 1].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[i - 1] / m_totalSize) * paintRect.width()), 0));

                if (m_pathInfo.at(i) == "unallocated") {
                    painter.setBrush(QBrush(color));
                    painter.setPen(QPen(color, 3));
                    painter.fillPath(path[i], color);
                } else {
                    painter.setBrush(QBrush(m_colorInfo[i % (m_colorInfo.size())]));
                    painter.setPen(QPen(QColor(m_colorInfo[i % (m_colorInfo.size())]), 3));
                    painter.fillPath(path[i], m_colorInfo[i % (m_colorInfo.size())]);
                }

                m_reectInfo.append(path[i].controlPointRect());
            } else if (i == m_sizeInfo.size() - 1) {
                path[m_sizeInfo.size() - 1].moveTo(paintRect.bottomRight() - QPoint(0, radius));
                path[m_sizeInfo.size() - 1].lineTo(paintRect.topRight() + QPoint(0, radius));
                path[m_sizeInfo.size() - 1].arcTo(QRect(QPoint(paintRect.topRight() - QPoint(radius * 2, 0)),
                                                        QSize(radius * 2, radius * 2)),
                                                  0, 90);
                path[m_sizeInfo.size() - 1].lineTo(path[m_sizeInfo.size() - 2].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[m_sizeInfo.size() - 2] / m_totalSize) * paintRect.width()), 0));
                path[m_sizeInfo.size() - 1].lineTo(path[m_sizeInfo.size() - 2].currentPosition() + QPoint(static_cast<int>((m_sizeInfo[m_sizeInfo.size() - 2] / m_totalSize) * paintRect.width()), paintRect.height() - 1));
                path[m_sizeInfo.size() - 1].lineTo(paintRect.bottomRight() - QPoint(radius, 0));
                path[m_sizeInfo.size() - 1].arcTo(QRect(QPoint(paintRect.bottomRight() - QPoint(radius * 2, radius * 2)),
                                                        QSize(radius * 2, radius * 2)),
                                                  270, 90);
                if (m_pathInfo.at(m_sizeInfo.size() - 1) == "unallocated") {
                    painter.setBrush(QBrush(color));
                    painter.setPen(QPen(color, 3));
                    painter.fillPath(path[m_sizeInfo.size() - 1], color);
                } else {
                    painter.setBrush(QBrush(m_colorInfo[(m_sizeInfo.size() - 1) % (m_colorInfo.size())]));
                    painter.setPen(QPen(QColor(m_colorInfo[(m_sizeInfo.size() - 1) % (m_colorInfo.size())]), 3));
                    painter.fillPath(path[m_sizeInfo.size() - 1], m_colorInfo[(m_sizeInfo.size() - 1) % (m_colorInfo.size())]);
                }

                m_reectInfo.append(path[m_sizeInfo.size() - 1].controlPointRect());
            }
        }
    }

    int height = 90 - static_cast<int>((QApplication::font().pointSizeF() / 0.75 - 14) * 1);
    QRect roundRect = QRect(rect.bottomLeft().x() + 2, rect.bottomLeft().y() - height, 15, 15);
    QPainterPath painterPath;
    painterPath.addRoundedRect(roundRect, 3, 3);
    m_parentPb = DApplicationHelper::instance()->palette(this);
    QBrush brush = DApplicationHelper::instance()->palette(this).dark();
    QColor colorRect = m_parentPb.color(DPalette::Normal, DPalette::Dark);
    painter.setBrush(brush);
    painter.setPen(colorRect);
    painter.drawPath(painterPath);
    painter.fillPath(painterPath, brush);

    QFont font;
    font = DFontSizeManager::instance()->get(DFontSizeManager::T7);
    QColor textColor = m_parentPb.color(DPalette::Normal, DPalette::Text);
    painter.setFont(font);
    painter.setPen(textColor);
    QTextOption option;
    option.setAlignment(Qt::AlignTop);
    QRect rectText = QRect(paintRect.bottomLeft().x() + 28, paintRect.bottomLeft().y() + 17, paintRect.width(), 70);
    painter.drawText(rectText, QString(tr("Unallocated")), option);

    painter.restore();
}

void PartitionInfoWidget::mouseMoveEvent(QMouseEvent *event)
{
    int x = event->pos().x();
    int y = event->pos().y();
    bool isInside = false;
    int curIndex = 0;
    for (int i = 0; i < m_reectInfo.count(); i++) {
        qreal rectX = m_reectInfo.at(i).x();
        qreal rectY = m_reectInfo.at(i).y();
        qreal rectWidth = m_reectInfo.at(i).width();
        qreal rectHeight = m_reectInfo.at(i).height();
        if (x > rectX && x < (rectX + rectWidth) && y > rectY && y < (rectY + rectHeight)) {
            curIndex = i;
            isInside = true;
            break;
        }
    }

    if (isInside) {
        emit enterWidget(m_reectInfo.at(curIndex), m_pathInfo.at(curIndex));
    } else {
        emit leaveWidget();
    }
}

void PartitionInfoWidget::onHandleChangeTheme()
{
    m_parentPb = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
}
