/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtQuickPath module
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qquickpathitem_p_p.h"
#include "qnvprrendernode_p.h"
#include "qquickpathrendernode_p.h"
#include <QSGRendererInterface>
#include <QPainterPath>

QT_BEGIN_NAMESPACE

QQuickPathItem::QQuickPathItem(QQuickItem *parent)
    : QQuickItem(*new QQuickPathItemPrivate, parent)
{
    setFlag(ItemHasContents);
}

QQuickPathItem::~QQuickPathItem()
{
}

QSGNode *QQuickPathItemPrivate::updatePaintNode(QQuickItem *item, QSGNode *node)
{
    if (!node) {
        QSGRendererInterface *ri = item->window()->rendererInterface();
        if (!ri)
            return nullptr;
        const bool hasFill = fillColor != Qt::transparent;
        const bool hasStroke = !qFuzzyIsNull(strokeWidth) && strokeColor != Qt::transparent;
        switch (ri->graphicsApi()) {
#ifndef QT_NO_OPENGL
            case QSGRendererInterface::OpenGL:
//                if (QNvprRenderNode::isSupported()) {
//                    node = new QNvprRenderNode(item);
//                    renderer = new QNvprPathRenderer(static_cast<QNvprRenderNode *>(node));
//                } else {
                    node = new QQuickPathRootRenderNode(item, hasFill, hasStroke);
                    renderer = new QQuickPathRenderer(static_cast<QQuickPathRootRenderNode *>(node));
//                }
                break;
#endif

            case QSGRendererInterface::Direct3D12:
                node = new QQuickPathRootRenderNode(item, hasFill, hasStroke);
                renderer = new QQuickPathRenderer(static_cast<QQuickPathRootRenderNode *>(node));
                break;

            case QSGRendererInterface::Software:
            default:
                qWarning("No path backend for this graphics API yet");
                break;
        }
        dirty |= 0xFFFF;
    }

    renderer->beginSync();

    if (dirty & QQuickPathItemPrivate::DirtyPath)
        renderer->setPath(path);
    if (dirty & QQuickPathItemPrivate::DirtyFillColor)
        renderer->setFillColor(fillColor, fillGradient);
    if (dirty & QQuickPathItemPrivate::DirtyStrokeColor)
        renderer->setStrokeColor(strokeColor);
    if (dirty & QQuickPathItemPrivate::DirtyStrokeWidth)
        renderer->setStrokeWidth(strokeWidth);
    if (dirty & QQuickPathItemPrivate::DirtyFlags)
        renderer->setFlags(flags);
    if (dirty & QQuickPathItemPrivate::DirtyStyle) {
        renderer->setJoinStyle(joinStyle, miterLimit);
        renderer->setCapStyle(capStyle);
        renderer->setStrokeStyle(strokeStyle, dashOffset, dashPattern, cosmeticStroke);
    }

    renderer->endSync();
    dirty = 0;

    return node;
}

QSGNode *QQuickPathItem::updatePaintNode(QSGNode *node, UpdatePaintNodeData *)
{
    Q_D(QQuickPathItem);
    return d->updatePaintNode(this, node);
}

void QQuickPathItem::clear()
{
    Q_D(QQuickPathItem);
    d->path = QPainterPath();
    d->dirty |= QQuickPathItemPrivate::DirtyPath;
}

bool QQuickPathItem::isEmpty() const
{
    Q_D(const QQuickPathItem);
    return d->path.isEmpty();
}

void QQuickPathItem::closeSubPath()
{
    Q_D(QQuickPathItem);
    d->path.closeSubpath();
    d->dirty |= QQuickPathItemPrivate::DirtyPath;
}

void QQuickPathItem::moveTo(qreal x, qreal y)
{
    Q_D(QQuickPathItem);
    d->path.moveTo(x, y);
    d->dirty |= QQuickPathItemPrivate::DirtyPath;
}

void QQuickPathItem::lineTo(qreal x, qreal y)
{
    Q_D(QQuickPathItem);
    d->path.lineTo(x, y);
    d->dirty |= QQuickPathItemPrivate::DirtyPath;
}

void QQuickPathItem::arcMoveTo(qreal x, qreal y, qreal w, qreal h, qreal angle)
{
    Q_D(QQuickPathItem);
    d->path.arcMoveTo(x, y, w, h, angle);
    d->dirty |= QQuickPathItemPrivate::DirtyPath;
}

void QQuickPathItem::arcTo(qreal x, qreal y, qreal w, qreal h, qreal startAngle, qreal arcLength)
{
    Q_D(QQuickPathItem);
    d->path.arcTo(x, y, w, h, startAngle, arcLength);
    d->dirty |= QQuickPathItemPrivate::DirtyPath;
}

void QQuickPathItem::cubicTo(qreal c1x, qreal c1y, qreal c2x, qreal c2y, qreal endX, qreal endY)
{
    Q_D(QQuickPathItem);
    d->path.cubicTo(c1x, c1y, c2x, c2y, endX, endY);
    d->dirty |= QQuickPathItemPrivate::DirtyPath;
}

void QQuickPathItem::quadTo(qreal cX, qreal cY, qreal endX, qreal endY)
{
    Q_D(QQuickPathItem);
    d->path.quadTo(cX, cY, endX, endY);
    d->dirty |= QQuickPathItemPrivate::DirtyPath;
}

void QQuickPathItem::addRect(qreal x, qreal y, qreal w, qreal h)
{
    Q_D(QQuickPathItem);
    d->path.addRect(x, y, w, h);
    d->dirty |= QQuickPathItemPrivate::DirtyPath;
}

void QQuickPathItem::addRoundedRect(qreal x, qreal y, qreal w, qreal h, qreal xr, qreal yr)
{
    Q_D(QQuickPathItem);
    d->path.addRoundedRect(x, y, w, h, xr, yr);
    d->dirty |= QQuickPathItemPrivate::DirtyPath;
}

void QQuickPathItem::addEllipse(qreal x, qreal y, qreal rx, qreal ry)
{
    Q_D(QQuickPathItem);
    d->path.addEllipse(x, y, rx, ry);
    d->dirty |= QQuickPathItemPrivate::DirtyPath;
}

void QQuickPathItem::addEllipseWithCenter(qreal cx, qreal cy, qreal rx, qreal ry)
{
    Q_D(QQuickPathItem);
    d->path.addEllipse(QPointF(cx, cy), rx, ry);
    d->dirty |= QQuickPathItemPrivate::DirtyPath;
}

QPointF QQuickPathItem::currentPosition() const
{
    Q_D(const QQuickPathItem);
    return d->path.currentPosition();
}

QRectF QQuickPathItem::boundingRect() const
{
    Q_D(const QQuickPathItem);
    return d->path.boundingRect();
}

QRectF QQuickPathItem::controlPointRect() const
{
    Q_D(const QQuickPathItem);
    return d->path.controlPointRect();
}

QQuickPathItem::FillRule QQuickPathItem::fillRule() const
{
    Q_D(const QQuickPathItem);
    return FillRule(d->path.fillRule());
}

void QQuickPathItem::setFillRule(FillRule fillRule)
{
    Q_D(QQuickPathItem);
    if (d->path.fillRule() != Qt::FillRule(fillRule)) {
        d->path.setFillRule(Qt::FillRule(fillRule));
        d->dirty |= QQuickPathItemPrivate::DirtyPath;
        emit fillRuleChanged();
        update();
    }
}

QColor QQuickPathItem::fillColor() const
{
    Q_D(const QQuickPathItem);
    return d->fillColor;
}

void QQuickPathItem::setFillColor(const QColor &color)
{
    Q_D(QQuickPathItem);
    if (d->fillColor != color) {
        d->fillColor = color;
        d->dirty |= QQuickPathItemPrivate::DirtyFillColor;
        emit fillColorChanged();
        update();
    }
}

QQuickPathGradient *QQuickPathItem::fillGradient() const
{
    Q_D(const QQuickPathItem);
    return d->fillGradient;
}

void QQuickPathItem::setFillGradient(QQuickPathGradient *gradient)
{
    Q_D(QQuickPathItem);
    if (d->fillGradient != gradient) {
        d->fillGradient = gradient;
        d->dirty |= QQuickPathItemPrivate::DirtyFillColor;
        update();
    }
}

void QQuickPathItem::resetFillGradient()
{
    setFillGradient(nullptr);
}

QColor QQuickPathItem::strokeColor() const
{
    Q_D(const QQuickPathItem);
    return d->strokeColor;
}

void QQuickPathItem::setStrokeColor(const QColor &color)
{
    Q_D(QQuickPathItem);
    if (d->strokeColor != color) {
        d->strokeColor = color;
        d->dirty |= QQuickPathItemPrivate::DirtyStrokeColor;
        emit strokeColorChanged();
        update();
    }
}

qreal QQuickPathItem::strokeWidth() const
{
    Q_D(const QQuickPathItem);
    return d->strokeWidth;
}

void QQuickPathItem::setStrokeWidth(qreal w)
{
    Q_D(QQuickPathItem);
    if (d->strokeWidth != w) {
        d->strokeWidth = w;
        d->dirty |= QQuickPathItemPrivate::DirtyStrokeWidth;
        emit strokeWidthChanged();
        update();
    }
}

QQuickPathItem::JoinStyle QQuickPathItem::joinStyle() const
{
    Q_D(const QQuickPathItem);
    return d->joinStyle;
}

void QQuickPathItem::setJoinStyle(JoinStyle style)
{
    Q_D(QQuickPathItem);
    if (d->joinStyle != style) {
        d->joinStyle = style;
        d->dirty |= QQuickPathItemPrivate::DirtyStyle;
        emit joinStyleChanged();
        update();
    }
}

int QQuickPathItem::miterLimit() const
{
    Q_D(const QQuickPathItem);
    return d->miterLimit;
}

void QQuickPathItem::setMiterLimit(int limit)
{
    Q_D(QQuickPathItem);
    if (d->miterLimit != limit) {
        d->miterLimit = limit;
        d->dirty |= QQuickPathItemPrivate::DirtyStyle;
        emit miterLimitChanged();
        update();
    }
}

QQuickPathItem::CapStyle QQuickPathItem::capStyle() const
{
    Q_D(const QQuickPathItem);
    return d->capStyle;
}

void QQuickPathItem::setCapStyle(CapStyle style)
{
    Q_D(QQuickPathItem);
    if (d->capStyle != style) {
        d->capStyle = style;
        d->dirty |= QQuickPathItemPrivate::DirtyStyle;
        emit capStyleChanged();
        update();
    }
}

QQuickPathItem::StrokeStyle QQuickPathItem::strokeStyle() const
{
    Q_D(const QQuickPathItem);
    return d->strokeStyle;
}

void QQuickPathItem::setStrokeStyle(StrokeStyle style)
{
    Q_D(QQuickPathItem);
    if (d->strokeStyle != style) {
        d->strokeStyle = style;
        d->dirty |= QQuickPathItemPrivate::DirtyStyle;
        emit strokeStyleChanged();
        update();
    }
}

qreal QQuickPathItem::dashOffset() const
{
    Q_D(const QQuickPathItem);
    return d->dashOffset;
}

void QQuickPathItem::setDashOffset(qreal offset)
{
    Q_D(QQuickPathItem);
    if (d->dashOffset != offset) {
        d->dashOffset = offset;
        d->dirty |= QQuickPathItemPrivate::DirtyStyle;
        emit dashOffsetChanged();
        update();
    }
}

QVector<qreal> QQuickPathItem::dashPattern() const
{
    Q_D(const QQuickPathItem);
    return d->dashPattern;
}

void QQuickPathItem::setDashPattern(const QVector<qreal> &array)
{
    Q_D(QQuickPathItem);
    if (d->dashPattern != array) {
        d->dashPattern = array;
        d->dirty |= QQuickPathItemPrivate::DirtyStyle;
        emit dashPatternChanged();
        update();
    }
}

bool QQuickPathItem::isCosmeticStroke() const
{
    Q_D(const QQuickPathItem);
    return d->cosmeticStroke;
}

void QQuickPathItem::setCosmeticStroke(bool cosmetic)
{
    Q_D(QQuickPathItem);
    if (d->cosmeticStroke != cosmetic) {
        d->cosmeticStroke = cosmetic;
        d->dirty |= QQuickPathItemPrivate::DirtyStyle;
        emit cosmeticStrokeChanged();
        update();
    }
}

QQuickPathGradientStop::QQuickPathGradientStop(QObject *parent)
    : QObject(parent),
      m_position(0),
      m_color(Qt::black)
{
}

qreal QQuickPathGradientStop::position() const
{
    return m_position;
}

void QQuickPathGradientStop::setPosition(qreal position)
{
    m_position = position;
}

QColor QQuickPathGradientStop::color() const
{
    return m_color;
}

void QQuickPathGradientStop::setColor(const QColor &color)
{
    m_color = color;
}

QQuickPathGradient::QQuickPathGradient(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QQuickPathGradientStop> QQuickPathGradient::stops()
{
    return QQmlListProperty<QQuickPathGradientStop>(this, m_stops);
}

QT_END_NAMESPACE
