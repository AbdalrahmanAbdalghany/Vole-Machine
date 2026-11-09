// ============================================================================
//  Vole Machine - Virtual Processor Simulator  (Qt Widgets GUI)
//
//  This file REPLACES Main.cpp / clsConsoleUI.h only as the entry point.
//  Your original files (Cpu.h, Memory.h, RegesterFile.h, Instructions.h/.cpp,
//  Decoder.h, ...) are used exactly as they are - nothing in them is changed.
// ============================================================================
#include <QtWidgets>
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QWindow>
#include <QGuiApplication>
#include <QScreen>
#include <QDateTime>
#include <functional>
#include <string>
#include <vector>

// ---- your original project (unchanged) ------------------------------------
#include "Cpu.h"
#include "Memory.h"
#include "RegesterFile.h"
#include "Instructions.h"
#include "Decoder.h"
#include "clsInputValidate.h"
#include "VoleMechine.h"   // your loader + WriteMemory / WriteRegester
// ----------------------------------------------------------------------------

namespace {

    QString hx(int v, int w = 2)
    {
        return QString::number(v, 16).toUpper().rightJustified(w, QLatin1Char('0'));
    }

    bool isHexCh(QChar c)
    {
        ushort u = c.unicode();
        return (u >= '0' && u <= '9') || (u >= 'a' && u <= 'f') || (u >= 'A' && u <= 'F');
    }

    void repolish(QWidget* w)
    {
        w->style()->unpolish(w);
        w->style()->polish(w);
        w->update();
    }


    enum Ik {
        IkCode, IkChip, IkMem, IkHist, IkGear, IkMin, IkMax, IkRestore, IkClose,
        IkLoad, IkRun, IkStep, IkReset, IkStop, IkConsole, IkPulse, IkResult, IkX, IkArrow
    };

    // Crisp vector icons drawn at runtime (no image files, no font glyphs).
    QPixmap iconPix(Ik k, const QColor& c, int sz)
    {
        const qreal dpr = 2.0;
        QPixmap pm(int(sz * dpr), int(sz * dpr));
        pm.setDevicePixelRatio(dpr);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.scale(sz / 24.0, sz / 24.0);                       // design grid: 24 x 24
        QPen pen(c, (sz <= 16 ? 2.4 : 2.0), Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        p.setPen(pen);
        p.setBrush(Qt::NoBrush);
        auto line = [&](qreal a, qreal b, qreal x, qreal y) { p.drawLine(QPointF(a, b), QPointF(x, y)); };
        auto poly = [&](std::initializer_list<QPointF> pts) {
            QVector<QPointF> v(pts);
            p.drawPolyline(v.constData(), v.size());
            };
        switch (k) {
        case IkCode:
            poly({ { 8, 6 },{ 2.5, 12 },{ 8, 18 } });
            poly({ { 16, 6 },{ 21.5, 12 },{ 16, 18 } });
            line(14, 4, 10, 20);
            break;
        case IkChip:
            p.drawRoundedRect(QRectF(6, 6, 12, 12), 2.5, 2.5);
            p.setBrush(c);
            p.drawRect(QRectF(10, 10, 4, 4));
            p.setBrush(Qt::NoBrush);
            for (qreal x : {9.0, 12.0, 15.0}) { line(x, 2.5, x, 6); line(x, 18, x, 21.5); }
            for (qreal y : {9.0, 12.0, 15.0}) { line(2.5, y, 6, y); line(18, y, 21.5, y); }
            break;
        case IkMem:
            for (qreal y : {3.5, 9.7, 15.9}) {
                p.drawRoundedRect(QRectF(3.5, y, 17, 4.8), 2, 2);
            }
            p.setBrush(c);
            for (qreal y : {5.9, 12.1, 18.3}) p.drawEllipse(QPointF(7, y), 0.9, 0.9);
            break;
        case IkHist:
            p.drawEllipse(QPointF(12, 12), 9, 9);
            poly({ { 12, 6.5 },{ 12, 12 },{ 16, 14.5 } });
            break;
        case IkGear: {
            p.translate(12, 12);
            QPen tooth(c, 3.6, Qt::SolidLine, Qt::FlatCap);
            for (int i = 0; i < 8; ++i) {
                p.save();
                p.rotate(i * 45.0);
                p.setPen(tooth);
                p.drawLine(QPointF(0, -8.0), QPointF(0, -10.6));
                p.restore();
            }
            p.setPen(pen);
            p.drawEllipse(QPointF(0, 0), 7.2, 7.2);
            p.drawEllipse(QPointF(0, 0), 2.8, 2.8);
            break;
        }
        case IkMin:
            line(6, 12, 18, 12);
            break;
        case IkMax:
            p.drawRoundedRect(QRectF(6, 6, 12, 12), 1.8, 1.8);
            break;
        case IkRestore:
            p.drawRoundedRect(QRectF(5.5, 9, 9.5, 9.5), 1.6, 1.6);
            poly({ { 9, 9 },{ 9, 6 },{ 18.5, 6 },{ 18.5, 15.5 },{ 15, 15.5 } });
            break;
        case IkClose:
            line(6, 6, 18, 18);
            line(18, 6, 6, 18);
            break;
        case IkLoad:
            line(12, 4, 12, 15);
            poly({ { 7, 10.5 },{ 12, 15.5 },{ 17, 10.5 } });
            poly({ { 5, 20 },{ 19, 20 } });
            break;
        case IkRun:
            p.setBrush(c);
            p.drawPolygon(QPolygonF({ { 8, 5 },{ 19, 12 },{ 8, 19 } }));
            break;
        case IkStep:
            p.drawPolygon(QPolygonF({ { 6, 5.5 },{ 16, 12 },{ 6, 18.5 } }));
            line(19.5, 5.5, 19.5, 18.5);
            break;
        case IkReset:
            p.drawArc(QRectF(5, 5, 14, 14), 40 * 16, 290 * 16);
            poly({ { 17.8, 3.2 },{ 17.8, 7.8 },{ 13.2, 7.8 } });
            break;
        case IkStop:
            p.setBrush(c);
            p.drawRoundedRect(QRectF(6, 6, 12, 12), 2.2, 2.2);
            break;
        case IkConsole:
            poly({ { 5, 7 },{ 11, 12 },{ 5, 17 } });
            line(13, 18, 19.5, 18);
            break;
        case IkPulse:
            poly({ { 2, 12 },{ 7, 12 },{ 10, 5 },{ 14, 19 },{ 17, 12 },{ 22, 12 } });
            break;
        case IkResult:
            p.drawEllipse(QPointF(12, 12), 9, 9);
            poly({ { 7.6, 12.4 },{ 10.7, 15.6 },{ 16.6, 8.8 } });
            break;
        case IkX:
            line(7.5, 7.5, 16.5, 16.5);
            line(16.5, 7.5, 7.5, 16.5);
            break;
        case IkArrow:
            line(4, 12, 19, 12);
            poly({ { 12.5, 5.5 },{ 19, 12 },{ 12.5, 18.5 } });
            break;
        }
        return pm;
    }

    QIcon mkIcon(Ik k, const QColor& normal, const QColor& active = QColor(), int sz = 22,
        const QColor& disabled = QColor("#565e8f"))
    {
        QIcon ic;
        const QColor on = active.isValid() ? active : normal;
        ic.addPixmap(iconPix(k, normal, sz), QIcon::Normal, QIcon::Off);
        ic.addPixmap(iconPix(k, on, sz), QIcon::Normal, QIcon::On);
        ic.addPixmap(iconPix(k, on, sz), QIcon::Active, QIcon::Off);
        ic.addPixmap(iconPix(k, on, sz), QIcon::Active, QIcon::On);
        ic.addPixmap(iconPix(k, disabled, sz), QIcon::Disabled, QIcon::Off);
        ic.addPixmap(iconPix(k, disabled, sz), QIcon::Disabled, QIcon::On);
        return ic;
    }

    // Human readable form of a 4-hex-digit instruction (display only).
    QString disasm(const QString& ir)
    {
        if (ir.size() < 4) return QStringLiteral("-");
        int d[4];
        for (int i = 0; i < 4; ++i) {
            bool ok = false;
            d[i] = ir.mid(i, 1).toInt(&ok, 16);
            if (!ok) return QStringLiteral("???");
        }
        auto R = [](int n) { return QString("R") + hx(n, 1); };
        const QString xy = hx(d[2] * 16 + d[3]);
        switch (d[0]) {
        case 1:  return QString("LOAD   %1, [%2]").arg(R(d[1]), xy);
        case 2:  return QString("LOAD   %1, #%2").arg(R(d[1]), xy);
        case 3:  return QString("STORE  %1, [%2]").arg(R(d[1]), xy);
        case 4:  return QString("MOVE   %1, %2").arg(R(d[2]), R(d[3]));
        case 5:  return QString("ADD    %1, %2, %3").arg(R(d[1]), R(d[2]), R(d[3]));
        case 6:  return QString("ADDF   %1, %2, %3").arg(R(d[1]), R(d[2]), R(d[3]));
        case 7:  return QString("OR     %1, %2, %3").arg(R(d[1]), R(d[2]), R(d[3]));
        case 8:  return QString("AND    %1, %2, %3").arg(R(d[1]), R(d[2]), R(d[3]));
        case 9:  return QString("XOR    %1, %2, %3").arg(R(d[1]), R(d[2]), R(d[3]));
        case 10: return QString("ROR    %1, %2").arg(R(d[1]), QString::number(d[3]));
        case 11: return QString("JMPEQ  %1, [%2]").arg(R(d[1]), xy);
        case 12: return QStringLiteral("HALT");
        default: return QStringLiteral("???");
        }
    }

    // Same parsing rules as VoleMechine::PropramLoader (comments after ';',
    // whitespace separated tokens, every 2 hex digits = 1 byte) + validation.
    std::vector<std::string> parseProgram(const QString& text, QString& err, QStringList& warns)
    {
        std::vector<std::string> out;
        const QStringList lines = text.split('\n');
        for (int ln = 0; ln < lines.size(); ++ln) {
            QString line = lines[ln];
            int sc = line.indexOf(';');
            if (sc >= 0) line = line.left(sc);
            const QStringList toks = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
            for (const QString& t : toks) {
                for (QChar c : t) {
                    if (!isHexCh(c)) {
                        err = QString("Line %1: invalid hex character '%2' in \"%3\".").arg(ln + 1).arg(c).arg(t);
                        return {};
                    }
                }
                if (t.size() % 2)
                    warns << QString("Line %1: \"%2\" has an odd number of digits - the last digit is ignored.")
                    .arg(ln + 1).arg(t);
                for (int i = 0; i + 1 < t.size(); i += 2)
                    out.push_back(t.mid(i, 2).toUpper().toStdString());
            }
        }
        if (out.size() % 2)
            warns << QStringLiteral("Program has an odd number of bytes - the last instruction is incomplete.");
        return out;
    }

    QString sampleProgram()
    {
        return QStringLiteral(
            "; Sample: R1 = 0x20, R2 = 0x21, R3 = R1 + R2 (= 0x41 'A')\n"
            "; then store R3 at address FF (the Screen) and halt.\n"
            "2120   ; R1 <- 20\n"
            "2221   ; R2 <- 21\n"
            "5312   ; R3 <- R1 + R2\n"
            "33FF   ; STORE R3 -> [FF]\n"
            "C000   ; HALT\n");
    }

} // namespace

// ============================================================================
//  Syntax highlighting for the hex editor
// ============================================================================
class HexHighlighter : public QSyntaxHighlighter {
public:
    explicit HexHighlighter(QTextDocument* d) : QSyntaxHighlighter(d)
    {
        fOp.setForeground(QColor("#c26bff"));
        fOp.setFontWeight(QFont::Bold);
        fA.setForeground(QColor("#2de2e6"));
        fB.setForeground(QColor("#ffd24a"));
        fCom.setForeground(QColor("#5d7b9c"));
        fCom.setFontItalic(true);
        fBad.setForeground(QColor("#ff4d6d"));
        fBad.setUnderlineStyle(QTextCharFormat::WaveUnderline);
        fBad.setUnderlineColor(QColor("#ff4d6d"));
    }

protected:
    void highlightBlock(const QString& text) override
    {
        int parity = (previousBlockState() == 1) ? 1 : 0; // byte index inside the instruction
        const int sc = text.indexOf(';');
        const int codeEnd = (sc < 0) ? text.size() : sc;
        if (sc >= 0) setFormat(sc, text.size() - sc, fCom);

        int i = 0;
        while (i < codeEnd) {
            if (text[i].isSpace()) { ++i; continue; }
            int s = i;
            while (i < codeEnd && !text[i].isSpace()) ++i;
            const int len = i - s;
            for (int k = 0; k < len; k += 2) {
                if (k + 1 >= len) { setFormat(s + k, 1, fBad); break; }
                if (!isHexCh(text[s + k]) || !isHexCh(text[s + k + 1])) {
                    setFormat(s + k, 2, fBad);
                }
                else if (parity == 0) {
                    setFormat(s + k, 1, fOp);       // opcode nibble
                    setFormat(s + k + 1, 1, fA);    // operand 1
                }
                else {
                    setFormat(s + k, 2, fB);        // operands 2,3 (XY)
                }
                parity ^= 1;
            }
        }
        setCurrentBlockState(parity);
    }

private:
    QTextCharFormat fOp, fA, fB, fCom, fBad;
};

// ============================================================================
//  Code editor with line numbers
// ============================================================================
class CodeEditor;

class LineNumberArea : public QWidget {
public:
    explicit LineNumberArea(CodeEditor* e);
    QSize sizeHint() const override;
protected:
    void paintEvent(QPaintEvent* ev) override;
private:
    CodeEditor* ed;
};

class CodeEditor : public QPlainTextEdit {
public:
    QString path;

    explicit CodeEditor(QWidget* parent = nullptr) : QPlainTextEdit(parent)
    {
        setObjectName("code");
        setLineWrapMode(QPlainTextEdit::NoWrap);
        area = new LineNumberArea(this);
        connect(this, &QPlainTextEdit::blockCountChanged, this,
            [this] { setViewportMargins(lineNumberAreaWidth(), 0, 0, 0); });
        connect(this, &QPlainTextEdit::updateRequest, this, [this](const QRect& r, int dy) {
            if (dy) area->scroll(0, dy);
            else area->update(0, r.y(), area->width(), r.height());
            if (r.contains(viewport()->rect())) setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
            });
        connect(this, &QPlainTextEdit::cursorPositionChanged, this, [this] { highlightLine(); });
        setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
        highlightLine();
    }

    int lineNumberAreaWidth() const
    {
        int digits = qMax(2, QString::number(qMax(1, blockCount())).size());
        return 26 + QFontMetrics(font()).horizontalAdvance(QLatin1Char('9')) * digits;
    }

    void paintNumbers(QPaintEvent* ev)
    {
        QPainter p(area);
        p.fillRect(ev->rect(), QColor("#0d0f28"));
        p.setFont(font());
        const QFontMetrics fm(font());
        QTextBlock block = firstVisibleBlock();
        int num = block.blockNumber();
        int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
        int bottom = top + qRound(blockBoundingRect(block).height());
        const int cur = textCursor().blockNumber();
        while (block.isValid() && top <= ev->rect().bottom()) {
            if (block.isVisible() && bottom >= ev->rect().top()) {
                p.setPen(num == cur ? QColor("#5aa2ff") : QColor("#5b6498"));
                p.drawText(0, top, area->width() - 14, fm.height(), Qt::AlignRight, QString::number(num + 1));
            }
            block = block.next();
            top = bottom;
            bottom = top + qRound(blockBoundingRect(block).height());
            ++num;
        }
    }

protected:
    void resizeEvent(QResizeEvent* e) override
    {
        QPlainTextEdit::resizeEvent(e);
        const QRect cr = contentsRect();
        area->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    }

private:
    void highlightLine()
    {
        QTextEdit::ExtraSelection sel;
        sel.format.setBackground(QColor("#13173f"));
        sel.format.setProperty(QTextFormat::FullWidthSelection, true);
        sel.cursor = textCursor();
        sel.cursor.clearSelection();
        setExtraSelections({ sel });
        area->update();
    }
    LineNumberArea* area;
};

LineNumberArea::LineNumberArea(CodeEditor* e) : QWidget(e), ed(e) {}
QSize LineNumberArea::sizeHint() const { return QSize(ed->lineNumberAreaWidth(), 0); }
void LineNumberArea::paintEvent(QPaintEvent* ev) { ed->paintNumbers(ev); }

// ============================================================================
//  Small UI building blocks
// ============================================================================
// ============================================================================
//  Cheap table styling: palette + custom header (stylesheet rules on tables
//  make scrolling slow, so tables only get a font rule in the stylesheet)
// ============================================================================
class FlatHeader : public QHeaderView {
public:
    explicit FlatHeader(Qt::Orientation o, QWidget* p = nullptr) : QHeaderView(o, p)
    {
        setSectionsClickable(false);
        setHighlightSections(false);
        setFixedHeight(32);
    }
protected:
    void paintSection(QPainter* p, const QRect& r, int sec) const override
    {
        p->save();
        p->fillRect(r, QColor("#12153f"));
        QFont f = font();
        f.setBold(true);
        p->setFont(f);
        p->setPen(QColor("#8e98c9"));

        const QVariant a = model()->headerData(sec, orientation(), Qt::TextAlignmentRole);
        const Qt::Alignment al = a.isValid() ? Qt::Alignment(a.toInt()) : Qt::Alignment(Qt::AlignCenter);
        QRect tr = r;
        if (al & Qt::AlignLeft)   // same margin as the cell text
            tr.adjust(style()->pixelMetric(QStyle::PM_FocusFrameHMargin, nullptr, this) + 1, 0, 0, 0);
        p->drawText(tr, int(al), model()->headerData(sec, orientation(), Qt::DisplayRole).toString());
        p->restore();
    }
};

static void styleTable(QTableView* t)
{
    t->setHorizontalHeader(new FlatHeader(Qt::Horizontal, t));
    t->setFrameShape(QFrame::NoFrame);
    QPalette pal = t->palette();
    pal.setColor(QPalette::Base, QColor("#0d0f2b"));
    pal.setColor(QPalette::AlternateBase, QColor("#11143a"));
    pal.setColor(QPalette::Text, QColor("#e8ecff"));
    pal.setColor(QPalette::Highlight, QColor("#2b2f8f"));
    pal.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    t->setPalette(pal);
    t->viewport()->setAutoFillBackground(true);
}

// ============================================================================
//  Memory table model: cheap to repaint (no per-cell item objects)
// ============================================================================
class MemModel : public QAbstractTableModel {
public:
    std::function<bool(int, const QString&)> onEdit;

    explicit MemModel(QObject* p = nullptr) : QAbstractTableModel(p)
    {
        for (int i = 0; i < 256; ++i) { val_[i] = QStringLiteral("00"); empty_[i] = true; until_[i] = 0; }
        tm_ = new QTimer(this);
        tm_->setInterval(100);
        connect(tm_, &QTimer::timeout, this, [this] { expire(); });
    }
    int rowCount(const QModelIndex & = QModelIndex()) const override { return 256; }
    int columnCount(const QModelIndex & = QModelIndex()) const override { return 4; }

    bool flashing() const { return tm_->isActive(); }

    QVariant headerData(int sec, Qt::Orientation o, int role) const override
    {
        if (o != Qt::Horizontal) return QVariant();
        if (role == Qt::DisplayRole) {
            static const char* h[] = { "Address", "Value (hex)", "ASCII", "Instruction" };
            return QString(h[sec]);
        }
        if (role == Qt::TextAlignmentRole)
            return int(sec == 3 ? (Qt::AlignLeft | Qt::AlignVCenter) : Qt::AlignCenter);
        return QVariant();
    }

    QVariant data(const QModelIndex& ix, int role) const override
    {
        const int r = ix.row(), c = ix.column();
        const bool fl = until_[r] > 0;
        switch (role) {
        case Qt::DisplayRole:
        case Qt::EditRole:
            if (c == 0) return hx(r);
            if (c == 1) return val_[r];
            if (c == 2) {
                bool ok = false;
                const int b = val_[r].toInt(&ok, 16);
                return (ok && b >= 32 && b < 127) ? QString(QChar(b)) : QString::fromUtf8("\u00B7");
            }
            return dec_[r];
        case Qt::UserRole:                       // the delegate draws the arrow when true
            return c == 0 && fl;
        case Qt::TextAlignmentRole:
            return int(c == 3 ? (Qt::AlignLeft | Qt::AlignVCenter) : Qt::AlignCenter);
        case Qt::ForegroundRole:
            if (c == 0) return fl ? QColor("#3dffa0") : QColor("#7f8fe0");
            if (c == 1) return fl ? QColor("#ffffff") : (empty_[r] ? QColor("#4c5586") : QColor("#e8ecff"));
            return fl ? QColor("#c9f5e4") : QColor("#8e98c9");
        case Qt::BackgroundRole:
            if (fl) return (c == 1) ? QColor("#1b6b4c") : QColor("#143d3a");
            return QVariant();
        }
        return QVariant();
    }

    Qt::ItemFlags flags(const QModelIndex& ix) const override
    {
        Qt::ItemFlags f = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        if (ix.column() == 1) f |= Qt::ItemIsEditable;
        return f;
    }

    bool setData(const QModelIndex& ix, const QVariant& v, int role) override
    {
        if (role != Qt::EditRole || ix.column() != 1 || !onEdit) return false;
        return onEdit(ix.row(), v.toString());
    }

    // Returns the last row that was written to (or -1 if nothing was written)
    int setState(const QString* nv, const bool* ne, const QString* nd, int newPc, bool hl)
    {
        int lo = 256, hi = -1, lastWrite = -1;
        auto mark = [&](int r) { if (r < 0 || r > 255) return; lo = qMin(lo, r); hi = qMax(hi, r); };
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        bool anyNew = false;
        for (int i = 0; i < 256; ++i) {
            const bool wr = init_ && hl && nv[i] != val_[i];
            if (wr) { until_[i] = now + kFlashMs; anyNew = true; lastWrite = i; mark(i); }
            if (nv[i] != val_[i] || ne[i] != empty_[i] || nd[i] != dec_[i]) mark(i);
            val_[i] = nv[i];
            empty_[i] = ne[i];
            dec_[i] = nd[i];
        }
        init_ = true;
        if (newPc != pc_) {
            mark(pc_); mark(pc_ + 1); mark(newPc); mark(newPc + 1);
            pc_ = newPc;
        }
        if (anyNew && !tm_->isActive()) tm_->start();
        if (hi >= 0) emit dataChanged(index(lo, 0), index(hi, 3));
        return lastWrite;
    }

private:
    static constexpr qint64 kFlashMs = 900;

    void expire()
    {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        int lo = 256, hi = -1;
        bool any = false;
        for (int i = 0; i < 256; ++i) {
            if (until_[i] <= 0) continue;
            if (now >= until_[i]) { until_[i] = 0; lo = qMin(lo, i); hi = qMax(hi, i); }
            else any = true;
        }
        if (!any) tm_->stop();
        if (hi >= 0) emit dataChanged(index(lo, 0), index(hi, 3));
    }

    QString val_[256], dec_[256];
    bool empty_[256] = {};
    qint64 until_[256] = {};
    int pc_ = -1;
    bool init_ = false;
    QTimer* tm_ = nullptr;
};

// Draws the arrow at the left of the Address column for the row that was just written
class ArrowDelegate : public QStyledItemDelegate {
public:
    explicit ArrowDelegate(QObject* p = nullptr)
        : QStyledItemDelegate(p), pm_(iconPix(IkArrow, QColor("#3dffa0"), 16)) {
    }
    void paint(QPainter* p, const QStyleOptionViewItem& o, const QModelIndex& ix) const override
    {
        QStyledItemDelegate::paint(p, o, ix);
        if (ix.data(Qt::UserRole).toBool())
            p->drawPixmap(o.rect.left() + 6, o.rect.top() + (o.rect.height() - 16) / 2, pm_);
    }
private:
    QPixmap pm_;
};

class Panel : public QFrame {
public:
    QLabel* titleLbl;

    Panel(int iconKind, const QString& title, QWidget* body, QWidget* parent = nullptr)
        : QFrame(parent), body_(body)
    {
        setObjectName("panel");
        auto* v = new QVBoxLayout(this);
        v->setContentsMargins(0, 0, 0, 0);
        v->setSpacing(0);

        header_ = new QWidget;
        header_->setObjectName("panelHeader");
        auto* h = new QHBoxLayout(header_);
        h->setContentsMargins(18, 12, 14, 10);
        h->setSpacing(10);
        auto* ic = new QLabel;
        ic->setObjectName("panelIcon");
        ic->setPixmap(iconPix(static_cast<Ik>(iconKind), QColor("#6d8bff"), 22));
        titleLbl = new QLabel(title);
        titleLbl->setObjectName("panelTitle");
        chev_ = new QToolButton;
        chev_->setObjectName("chev");
        chev_->setArrowType(Qt::UpArrow);
        chev_->setCursor(Qt::PointingHandCursor);
        h->addWidget(ic);
        h->addWidget(titleLbl);
        h->addStretch();
        h->addWidget(chev_);
        v->addWidget(header_);

        line_ = new QFrame;
        line_->setObjectName("hline");
        line_->setFixedHeight(1);
        v->addWidget(line_);
        v->addWidget(body, 1);

        connect(chev_, &QToolButton::clicked, this, [this] {
            const bool show = !body_->isVisible();
            body_->setVisible(show);
            line_->setVisible(show);
            chev_->setArrowType(show ? Qt::UpArrow : Qt::DownArrow);
            setMaximumHeight(show ? QWIDGETSIZE_MAX : header_->sizeHint().height() + 2);
            });
    }

private:
    QWidget* header_;
    QWidget* body_;
    QFrame* line_;
    QToolButton* chev_;
};

class TitleBar : public QWidget {
public:
    explicit TitleBar(QWidget* p) : QWidget(p)
    {
        setObjectName("titleBar");
        setAttribute(Qt::WA_StyledBackground, true);
    }
protected:
    void mousePressEvent(QMouseEvent* e) override
    {
        if (e->button() == Qt::LeftButton && window()->windowHandle())
            window()->windowHandle()->startSystemMove();
        QWidget::mousePressEvent(e);
    }
    void mouseDoubleClickEvent(QMouseEvent*) override
    {
        QWidget* w = window();
        if (w->isMaximized()) w->showNormal(); else w->showMaximized();
    }
};

// ============================================================================
//  Main window
// ============================================================================
class VoleWindow : public QWidget {
    enum class St { Ready, Running, Paused, Halted, Faulted };
    enum class Lg { Info, Ok, Warn, Err };
    struct Hist { long long n; int pc; QString ir; QString desc; };

public:
    VoleWindow()
    {
        setObjectName("root");
        setAttribute(Qt::WA_StyledBackground, true);
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        setWindowTitle("Vole Machine - Virtual Processor Simulator");
        setMinimumSize(1100, 700);
        const QRect av = QGuiApplication::primaryScreen()->availableGeometry();
        resize(qMin(1500, av.width() - 40), qMin(920, av.height() - 40));

        auto* outer = new QVBoxLayout(this);
        outer->setContentsMargins(0, 0, 0, 0);
        outer->setSpacing(0);
        outer->addWidget(buildTitleBar());

        auto* body = new QHBoxLayout;
        body->setContentsMargins(16, 10, 16, 16);
        body->setSpacing(16);
        outer->addLayout(body, 1);
        body->addWidget(buildSidebar());

        // ---- build all panels (order matters: log/console must exist early)
        QWidget* consoleBody = buildConsoleBody();
        consolePanel = new Panel(IkConsole, "Execution Console", consoleBody);
        editorPanel = buildEditorPanel();
        memPanel = new Panel(IkMem, "Memory", buildMemoryBody());
        regPanel = new Panel(IkChip, "Registers", buildRegisterBody());
        statusPanel = new Panel(IkPulse, "CPU Status", buildStatusBody());
        resultPanel = new Panel(IkResult, "Final Result", buildResultBody());
        QWidget* buttons = buildButtonBar();

        auto* g = new QGridLayout;
        g->setSpacing(14);
        gridLay = g;
        contentHost = new QWidget;
        contentHost->setLayout(g);
        g->setContentsMargins(0, 0, 0, 0);
        body->addWidget(contentHost, 1);

        auto* mid = new QWidget;
        auto* midL = new QVBoxLayout(mid);
        midL->setContentsMargins(0, 0, 0, 0);
        midL->setSpacing(14);
        midL->addWidget(resultPanel, 0);
        midL->addWidget(consolePanel, 1);

        auto* right = new QWidget;
        rightCol = right;
        auto* rightL = new QVBoxLayout(right);
        rightLay = rightL;
        rightL->setContentsMargins(0, 0, 0, 0);
        rightL->setSpacing(14);
        rightL->addWidget(regPanel, 0);
        rightL->addWidget(statusPanel, 0);
        rightL->addStretch(1);

        g->addWidget(editorPanel, 0, 0, 1, 2);
        g->addWidget(memPanel, 1, 0, 2, 1);
        g->addWidget(mid, 1, 1, 1, 1);
        g->addWidget(right, 0, 2, 2, 1);
        g->addWidget(buttons, 2, 1, 1, 2);
        g->setColumnStretch(0, 9);
        g->setColumnStretch(1, 10);
        g->setColumnStretch(2, 9);
        g->setRowStretch(0, 6);
        g->setRowStretch(1, 5);
        g->setRowStretch(2, 0);

        expAnim = new QVariantAnimation(this);
        expAnim->setDuration(200);
        expAnim->setEasingCurve(QEasingCurve::OutCubic);
        connect(expAnim, &QVariantAnimation::valueChanged, this, [this](const QVariant& v) {
            curT = v.toDouble();
            if (focusKind >= 0) applyExpand(focusKind, curT);
            });
        connect(expAnim, &QVariantAnimation::finished, this, [this] { onExpandDone(); });

        grip = new QSizeGrip(this);
        grip->setFixedSize(16, 16);

        setStyleSheet(styleSheet_());
        setupShortcuts();
        connect(&runTimer, &QTimer::timeout, this, [this] { onTick(); });

        newEditor("Program 1", sampleProgram());
        dirty = true;

        CPU::SetHalted(false);
        CPU::SetPC(startAddr());
        setState(St::Ready);
        refreshAll(false);
        log("Vole Machine ready.", Lg::Ok);
        log("Write your program in hex, then press Load and Run.", Lg::Info);
        log("F5 = Run     F10 = Step     Shift+F5 = Stop", Lg::Info);
        log("Ctrl+L = Load     Ctrl+Shift+R = Reset", Lg::Info);
    }

protected:
    void changeEvent(QEvent* e) override
    {
        QWidget::changeEvent(e);
        if (e->type() == QEvent::WindowStateChange && btnMax)
            btnMax->setIcon(mkIcon(isMaximized() ? IkRestore : IkMax, QColor("#c9d0ff"), QColor("#ffffff"), 20));
    }
    void resizeEvent(QResizeEvent* e) override
    {
        QWidget::resizeEvent(e);
        grip->move(width() - grip->width(), height() - grip->height());
        grip->raise();
    }

private:
    // ------------------------------------------------------------------ data
    St st = St::Ready;
    bool executing = false;   // an execution session is in progress
    bool dirty = true;        // editor changed since last load
    bool memInit = false;
    long long cycles = 0;
    long long budgetStart = 0;
    int delayMs = 60;
    int cycleLimit = 100000;
    bool autoLoad = true;
    int tabCounter = 1;

    QTimer runTimer;
    QVector<Hist> pending;
    QString memCache[256];
    int regCache[16] = {};
    QVector<int> hlRows;

    // ------------------------------------------------------------------ widgets
    QSizeGrip* grip = nullptr;
    QPushButton* btnMax = nullptr;
    QTabBar* tabBar = nullptr;
    QStackedWidget* editors = nullptr;
    QLineEdit* startEdit = nullptr;
    QLabel* posLbl = nullptr;
    QTableView* memTable = nullptr;
    MemModel* memModel = nullptr;
    QLineEdit* gotoEdit = nullptr;
    QLineEdit* regEdit[16] = {};
    QLabel* pcVal = nullptr, * irVal = nullptr, * cycVal = nullptr, * instrVal = nullptr, * badge = nullptr;
    QLabel* resHex = nullptr, * resDec = nullptr, * resBin = nullptr, * resAscii = nullptr, * resRegs = nullptr, * resMem = nullptr;
    int regStart[16] = {};
    // in-place expand / collapse (Registers grow taller, Memory grows wider)
    QWidget* contentHost = nullptr;
    QGridLayout* gridLay = nullptr;
    QVBoxLayout* rightLay = nullptr;
    QWidget* rightCol = nullptr;
    QGridLayout* statusGrid = nullptr;
    QVariantAnimation* expAnim = nullptr;
    double curT = 0.0;             // 0 = normal layout, 1 = fully expanded
    int expTarget = 0;             // where the animation is heading (0 / 1)
    bool memBig = false;
    int expStatusH = 0, expStatusCompactH = 0, expRegH = 0, expRegTargetH = 0, expEditH = 30;
    QGridLayout* regGrid = nullptr;
    QLabel* regLbl[16] = {};
    QLabel* regDec[16] = {};
    QLabel* regBin[16] = {};
    int focusKind = -1;            // -1 none, 0 registers, 1 memory
    QString memStart[256];
    QPlainTextEdit* console = nullptr;
    QTableWidget* history = nullptr;
    QStackedWidget* logStack = nullptr;
    QPushButton* btnLoad = nullptr, * btnRun = nullptr, * btnStep = nullptr, * btnReset = nullptr, * btnStop = nullptr;
    QPushButton* navEditor = nullptr, * navRegs = nullptr, * navMem = nullptr, * navHist = nullptr, * navSet = nullptr;
    QButtonGroup* navGroup = nullptr;
    QAbstractButton* lastNav = nullptr;
    Panel* consolePanel = nullptr, * memPanel = nullptr, * regPanel = nullptr, * statusPanel = nullptr, * resultPanel = nullptr;
    QFrame* editorPanel = nullptr;

    // ------------------------------------------------------------------ helpers
    CodeEditor* curEditor() const { return static_cast<CodeEditor*>(editors->currentWidget()); }

    int startAddr() const
    {
        bool ok = false;
        int v = startEdit ? startEdit->text().toInt(&ok, 16) : 0;
        if (!ok) v = 0;
        return qBound(0, v, 255);
    }

    void flash(QWidget* w)
    {
        w->setProperty("flash", true);
        repolish(w);
        QTimer::singleShot(700, w, [w] { w->setProperty("flash", false); repolish(w); });
    }

    void log(const QString& msg, Lg k = Lg::Info)
    {
        const char* col = "#e8ecff";
        if (k == Lg::Ok) col = "#3dffa0";
        else if (k == Lg::Warn) col = "#ffd24a";
        else if (k == Lg::Err) col = "#ff6b85";
        console->appendHtml(
            QString("<span style='white-space:pre; color:#7d8bb5'>&gt;  </span>"
                "<span style='white-space:pre; color:%1'>%2</span>")
            .arg(col, msg.toHtmlEscaped()));
    }

    // ------------------------------------------------------------------ UI: title bar
    QWidget* buildTitleBar()
    {
        auto* bar = new TitleBar(this);
        bar->setFixedHeight(64);
        auto* h = new QHBoxLayout(bar);
        h->setContentsMargins(18, 8, 6, 8);
        h->setSpacing(14);

        auto* logo = new QLabel("V");
        logo->setObjectName("logo");
        logo->setAlignment(Qt::AlignCenter);
        logo->setFixedSize(44, 44);
        auto* name = new QLabel("Vole Machine");
        name->setObjectName("appName");
        auto* sep = new QFrame;
        sep->setObjectName("vsep");
        sep->setFixedSize(1, 24);
        auto* sub = new QLabel("Virtual Processor Simulator");
        sub->setObjectName("appSub");
        h->addWidget(logo);
        h->addSpacing(4);
        h->addWidget(name);
        h->addWidget(sep);
        h->addWidget(sub);
        h->addStretch();

        auto mk = [&](Ik k, const char* id) {
            auto* b = new QPushButton;
            b->setObjectName(id);
            b->setIcon(mkIcon(k, QColor("#c9d0ff"), QColor("#ffffff"), 20));
            b->setIconSize(QSize(20, 20));
            b->setFixedSize(58, 44);
            b->setCursor(Qt::PointingHandCursor);
            b->setFocusPolicy(Qt::NoFocus);
            h->addWidget(b);
            return b;
            };
        auto* bMin = mk(IkMin, "winBtn");
        btnMax = mk(IkMax, "winBtn");
        auto* bClose = mk(IkClose, "closeBtn");
        connect(bMin, &QPushButton::clicked, this, [this] { showMinimized(); });
        connect(btnMax, &QPushButton::clicked, this, [this] { isMaximized() ? showNormal() : showMaximized(); });
        connect(bClose, &QPushButton::clicked, this, [this] { close(); });
        return bar;
    }

    // ------------------------------------------------------------------ UI: sidebar
    QWidget* buildSidebar()
    {
        auto* sb = new QFrame;
        sb->setObjectName("sidebar");
        sb->setFixedWidth(210);
        auto* v = new QVBoxLayout(sb);
        v->setContentsMargins(10, 14, 10, 12);
        v->setSpacing(6);
        navGroup = new QButtonGroup(this);
        navGroup->setExclusive(true);

        auto mk = [&](Ik k, const QString& text) {
            auto* b = new QPushButton(QStringLiteral("  ") + text);
            b->setObjectName("nav");
            b->setIcon(mkIcon(k, QColor("#b7c0ec"), QColor("#ffffff"), 22));
            b->setIconSize(QSize(22, 22));
            b->setCheckable(true);
            b->setCursor(Qt::PointingHandCursor);
            b->setFocusPolicy(Qt::NoFocus);
            b->setMinimumHeight(48);
            navGroup->addButton(b);
            v->addWidget(b);
            return b;
            };
        navEditor = mk(IkCode, "Editor");
        navRegs = mk(IkChip, "Registers");
        navMem = mk(IkMem, "Memory");
        navHist = mk(IkHist, "Execution History");
        navSet = mk(IkGear, "Settings");
        navEditor->setChecked(true);
        lastNav = navEditor;

        v->addStretch();
        auto* mark = new QLabel("V");
        mark->setObjectName("watermark");
        mark->setAlignment(Qt::AlignCenter);
        v->addWidget(mark);
        auto* ver = new QLabel("v1.0.0");
        ver->setObjectName("dim");
        ver->setAlignment(Qt::AlignCenter);
        v->addWidget(ver);

        connect(navEditor, &QPushButton::clicked, this, [this] {
            lastNav = navEditor;
            closeFocus();
            showLogView(0);
            flash(editorPanel);
            curEditor()->setFocus();
            });
        connect(navRegs, &QPushButton::clicked, this, [this] {
            lastNav = navRegs;
            toggleFocus(0);
            });
        connect(navMem, &QPushButton::clicked, this, [this] {
            lastNav = navMem;
            toggleFocus(1);
            });
        connect(navHist, &QPushButton::clicked, this, [this] {
            lastNav = navHist;
            closeFocus();
            showLogView(1);
            flash(consolePanel);
            });
        connect(navSet, &QPushButton::clicked, this, [this] {
            showSettings();
            if (lastNav) lastNav->setChecked(true);
            });
        return sb;
    }

    void showLogView(int idx)
    {
        logStack->setCurrentIndex(idx);
        consolePanel->titleLbl->setText(idx == 0 ? "Execution Console" : "Execution History");
    }

    // ------------------------------------------------------------------ UI: editor
    QFrame* buildEditorPanel()
    {
        auto* p = new QFrame;
        p->setObjectName("panel");
        auto* v = new QVBoxLayout(p);
        v->setContentsMargins(0, 0, 0, 0);
        v->setSpacing(0);

        auto* strip = new QWidget;
        auto* sh = new QHBoxLayout(strip);
        sh->setContentsMargins(10, 8, 14, 0);
        sh->setSpacing(6);
        tabBar = new QTabBar;
        tabBar->setExpanding(false);
        tabBar->setDrawBase(false);
        tabBar->setFocusPolicy(Qt::NoFocus);
        auto* plus = new QToolButton;
        plus->setObjectName("tbtn");
        plus->setText("+");
        plus->setToolTip("New program tab");
        auto* open = new QToolButton;
        open->setObjectName("tbtn");
        open->setText("Open");
        open->setToolTip("Open a program file (.txt)");
        auto* save = new QToolButton;
        save->setObjectName("tbtn");
        save->setText("Save");
        save->setToolTip("Save the current program");
        sh->addWidget(tabBar);
        sh->addWidget(plus);
        sh->addStretch();
        for (QToolButton* b : { open, save }) b->setFixedHeight(34);
        sh->addWidget(open, 0, Qt::AlignVCenter);
        sh->addWidget(save, 0, Qt::AlignVCenter);
        v->addWidget(strip);

        auto* line = new QFrame;
        line->setObjectName("hline");
        line->setFixedHeight(1);
        v->addWidget(line);

        editors = new QStackedWidget;
        v->addWidget(editors, 1);

        auto* foot = new QFrame;
        foot->setObjectName("footer");
        auto* fh = new QHBoxLayout(foot);
        fh->setContentsMargins(18, 6, 18, 6);
        fh->setSpacing(10);
        posLbl = new QLabel("Ln 1, Col 1");
        posLbl->setObjectName("dim");
        auto* loadAt = new QLabel("Load at (hex):");
        loadAt->setObjectName("dim");
        startEdit = new QLineEdit("00");
        startEdit->setObjectName("startEdit");
        startEdit->setMaxLength(2);
        startEdit->setFixedWidth(46);
        startEdit->setAlignment(Qt::AlignCenter);
        startEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9A-Fa-f]{0,2}"), startEdit));
        startEdit->setToolTip("Starting address (00 - FF) used by Load / Run / Reset");
        auto* enc = new QLabel("UTF-8");
        enc->setObjectName("dim");
        auto* lang = new QLabel("Vole");
        lang->setObjectName("dim");
        fh->addWidget(posLbl);
        fh->addStretch();
        fh->addWidget(loadAt);
        fh->addWidget(startEdit);
        fh->addSpacing(18);
        fh->addWidget(enc);
        fh->addSpacing(10);
        fh->addWidget(lang);
        v->addWidget(foot);

        connect(startEdit, &QLineEdit::editingFinished, this, [this] {
            startEdit->setText(hx(startAddr()));
            if (!executing) {
                CPU::SetPC(startAddr());
                try { CPU::SetIR(startAddr()); }
                catch (...) {}
                refreshAll(true);
            }
            });
        connect(tabBar, &QTabBar::currentChanged, this, [this](int i) {
            if (i >= 0) editors->setCurrentIndex(i);
            updatePos();
            });
        connect(plus, &QToolButton::clicked, this, [this] { newEditor(QString("Program %1").arg(++tabCounter), QString()); });
        connect(open, &QToolButton::clicked, this, [this] { openFile(); });
        connect(save, &QToolButton::clicked, this, [this] { saveFile(); });
        return p;
    }

    CodeEditor* newEditor(const QString& title, const QString& text)
    {
        auto* ed = new CodeEditor;
        new HexHighlighter(ed->document());
        ed->setPlainText(text);
        connect(ed, &QPlainTextEdit::textChanged, this, [this] { dirty = true; });
        connect(ed, &QPlainTextEdit::cursorPositionChanged, this, [this] { updatePos(); });
        const int idx = editors->addWidget(ed);
        tabBar->addTab(title);
        auto* x = new QToolButton;
        x->setObjectName("tabX");
        x->setIcon(mkIcon(IkX, QColor("#8e98c9"), QColor("#ff6b85"), 14));
        x->setIconSize(QSize(14, 14));
        x->setFixedSize(22, 22);
        x->setCursor(Qt::PointingHandCursor);
        x->setToolTip("Close tab");
        tabBar->setTabButton(idx, QTabBar::RightSide, x);
        connect(x, &QToolButton::clicked, this, [this, x] {
            for (int i = 0; i < tabBar->count(); ++i)
                if (tabBar->tabButton(i, QTabBar::RightSide) == x) { closeTab(i); return; }
            });
        tabBar->setCurrentIndex(idx);
        editors->setCurrentIndex(idx);
        dirty = true;
        return ed;
    }

    void closeTab(int i)
    {
        if (tabBar->count() <= 1) return;
        QWidget* w = editors->widget(i);
        editors->removeWidget(w);
        w->deleteLater();
        tabBar->removeTab(i);
        dirty = true;
    }

    void updatePos()
    {
        if (!editors->currentWidget() || !posLbl) return;
        const QTextCursor c = curEditor()->textCursor();
        posLbl->setText(QString("Ln %1, Col %2").arg(c.blockNumber() + 1).arg(c.columnNumber() + 1));
    }

    void openFile()
    {
        const QString f = QFileDialog::getOpenFileName(this, "Open program", QString(),
            "Vole programs (*.txt *.vole *.hex);;All files (*)");
        if (f.isEmpty()) return;
        QFile file(f);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            log("Failed to Load Program! File Not Found.", Lg::Err);
            return;
        }
        CodeEditor* ed = newEditor(QFileInfo(f).fileName(), QString::fromUtf8(file.readAll()));
        ed->path = f;
        log("Opened " + f, Lg::Info);
    }

    void saveFile()
    {
        CodeEditor* ed = curEditor();
        if (ed->path.isEmpty()) {
            const QString f = QFileDialog::getSaveFileName(this, "Save program", "program.txt", "Text files (*.txt);;All files (*)");
            if (f.isEmpty()) return;
            ed->path = f;
            tabBar->setTabText(tabBar->currentIndex(), QFileInfo(f).fileName());
        }
        QFile file(ed->path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            log("Could not write " + ed->path, Lg::Err);
            return;
        }
        file.write(ed->toPlainText().toUtf8());
        log("Saved " + ed->path, Lg::Ok);
    }

    // ------------------------------------------------------------------ UI: memory
    QWidget* buildMemoryBody()
    {
        auto* w = new QWidget;
        auto* v = new QVBoxLayout(w);
        v->setContentsMargins(12, 10, 12, 12);
        v->setSpacing(8);

        auto* top = new QHBoxLayout;
        gotoEdit = new QLineEdit;
        gotoEdit->setObjectName("startEdit");
        gotoEdit->setPlaceholderText("Go to address (hex) + Enter");
        gotoEdit->setMaxLength(2);
        gotoEdit->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9A-Fa-f]{0,2}"), gotoEdit));
        auto* clear = new QPushButton("Clear");
        clear->setObjectName("small");
        clear->setCursor(Qt::PointingHandCursor);
        clear->setToolTip("Clear all 256 memory cells");
        top->addWidget(gotoEdit, 1);
        top->addWidget(clear);
        v->addLayout(top);

        memModel = new MemModel(this);
        memModel->onEdit = [this](int row, const QString& txt) {
            const QString t = txt.trimmed();
            static const QRegularExpression re("^[0-9A-Fa-f]{1,2}$");
            if (!re.match(t).hasMatch()) {
                log("Memory value must be 1-2 hex digits (00 - FF).", Lg::Warn);
                return false;
            }
            MemoryEdit e;
            e.Address = row;
            e.Value = hx(t.toInt(nullptr, 16)).toStdString();
            VoleMechine::WriteMemory(e);
            QTimer::singleShot(0, this, [this] { refreshAll(false); });
            return true;
            };
        memTable = new QTableView;
        memTable->setObjectName("memTable");
        styleTable(memTable);
        memTable->setModel(memModel);
        memTable->setItemDelegateForColumn(0, new ArrowDelegate(memTable));
        memTable->verticalHeader()->hide();
        memTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        memTable->verticalHeader()->setDefaultSectionSize(30);
        memTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        memTable->horizontalHeader()->setHighlightSections(false);
        memTable->setAlternatingRowColors(true);
        memTable->setShowGrid(false);
        memTable->setWordWrap(false);
        memTable->setSelectionMode(QAbstractItemView::SingleSelection);
        memTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
        memTable->setFocusPolicy(Qt::StrongFocus);
        memTable->setColumnHidden(3, true);
        v->addWidget(memTable, 1);

        connect(gotoEdit, &QLineEdit::returnPressed, this, [this] {
            bool ok = false;
            const int a = gotoEdit->text().toInt(&ok, 16);
            if (!ok) return;
            memTable->setCurrentIndex(memModel->index(a, 1));
            memTable->scrollTo(memModel->index(a, 0), QAbstractItemView::PositionAtTop);
            });
        connect(clear, &QPushButton::clicked, this, [this] {
            if (QMessageBox::question(this, "Clear memory", "Clear all 256 memory cells?") != QMessageBox::Yes) return;
            for (int i = 0; i < 256; ++i) {
                MemoryEdit e;
                e.Address = i;
                e.Value = "";
                VoleMechine::WriteMemory(e);
            }
            dirty = true;                       // program is gone: reload it on the next Run / Step
            log("Memory cleared.", Lg::Warn);
            refreshAll(false);
            });
        return w;
    }

    // ------------------------------------------------------------------ UI: registers
    QWidget* buildRegisterBody()
    {
        auto* w = new QWidget;
        auto* g = new QGridLayout(w);
        g->setContentsMargins(18, 12, 18, 14);
        g->setHorizontalSpacing(12);
        g->setVerticalSpacing(6);
        regGrid = g;
        for (int i = 0; i < 16; ++i) {
            const int col = i / 8, row = i % 8;
            auto* lbl = new QLabel("R" + hx(i, 1));
            regLbl[i] = lbl;
            regDec[i] = new QLabel("= 0", w);
            regDec[i]->setObjectName("regDec");
            regDec[i]->hide();
            regBin[i] = new QLabel("0000 0000", w);
            regBin[i]->setObjectName("regBin");
            regBin[i]->hide();
            lbl->setObjectName("regLabel");
            lbl->setAlignment(Qt::AlignCenter);
            auto* e = new QLineEdit("00");
            e->setObjectName("reg");
            e->setMaxLength(2);
            e->setAlignment(Qt::AlignCenter);
            e->setMinimumHeight(30);
            e->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9A-Fa-f]{0,2}"), e));
            regEdit[i] = e;
            g->addWidget(lbl, row, col * 2);
            g->addWidget(e, row, col * 2 + 1);
            connect(e, &QLineEdit::editingFinished, this, [this, i] {
                bool ok = false;
                const int v = regEdit[i]->text().toInt(&ok, 16);
                if (ok && v != RegesterFile::Read(i)) {
                    RegisterEdit re;
                    re.Index = i;
                    re.Value = v;
                    VoleMechine::WriteRegester(re);
                }
                refreshAll(false);
                });
        }
        g->setColumnStretch(1, 1);
        g->setColumnStretch(3, 1);
        g->setColumnMinimumWidth(1, 70);
        g->setColumnMinimumWidth(3, 70);
        return w;
    }

    // ------------------------------------------------------------------ UI: CPU status
    QWidget* buildStatusBody()
    {
        auto* w = new QWidget;
        auto* g = new QGridLayout(w);
        statusGrid = g;
        g->setContentsMargins(22, 12, 18, 16);
        g->setHorizontalSpacing(16);
        g->setVerticalSpacing(8);
        auto name = [](const QString& t) { auto* l = new QLabel(t); l->setObjectName("statName"); return l; };
        auto val = [](const char* id) { auto* l = new QLabel("-"); l->setObjectName(id); return l; };
        pcVal = val("pcVal");
        irVal = val("irVal");
        cycVal = val("cycVal");
        instrVal = new QLabel("-");
        instrVal->setObjectName("instrVal");
        g->addWidget(name("PC"), 0, 0);
        g->addWidget(pcVal, 0, 1);
        g->addWidget(name("IR"), 1, 0);
        g->addWidget(irVal, 1, 1);
        g->addWidget(name("Cycles"), 2, 0);
        g->addWidget(cycVal, 2, 1);

        auto* sep = new QFrame;
        sep->setObjectName("vsep");
        sep->setFixedWidth(1);
        g->addWidget(sep, 0, 2, 3, 1);

        badge = new QLabel("Ready");
        badge->setObjectName("badge");
        badge->setAlignment(Qt::AlignCenter);
        badge->setMinimumHeight(52);
        g->addWidget(badge, 0, 3, 3, 1);
        g->addWidget(instrVal, 3, 0, 1, 4);
        g->setColumnStretch(1, 1);
        g->setColumnStretch(3, 2);
        return w;
    }

    // ------------------------------------------------------------------ UI: final result
    QWidget* buildResultBody()
    {
        auto* w = new QWidget;
        auto* g = new QGridLayout(w);
        g->setContentsMargins(16, 10, 16, 12);
        g->setHorizontalSpacing(16);
        g->setVerticalSpacing(3);
        resHex = new QLabel("--");
        resHex->setObjectName("resHex");
        resHex->setAlignment(Qt::AlignCenter);
        resHex->setMinimumSize(120, 70);
        resDec = new QLabel;
        resBin = new QLabel;
        resAscii = new QLabel;
        for (QLabel* l : { resDec, resBin, resAscii }) l->setObjectName("resLine");
        resRegs = new QLabel;
        resMem = new QLabel;
        for (QLabel* l : { resRegs, resMem }) {
            l->setObjectName("resNote");
            l->setWordWrap(true);
            l->setTextInteractionFlags(Qt::TextSelectableByMouse);
        }
        auto* cap = new QLabel("Screen  (address FF)");
        cap->setObjectName("dim");
        g->addWidget(resHex, 0, 0, 3, 1);
        g->addWidget(resDec, 0, 1);
        g->addWidget(resBin, 1, 1);
        g->addWidget(resAscii, 2, 1);
        g->addWidget(cap, 3, 0);
        g->addWidget(resRegs, 4, 0, 1, 2);
        g->addWidget(resMem, 5, 0, 1, 2);
        g->setColumnStretch(1, 1);
        return w;
    }

    // ------------------------------------------------------------------ UI: console + history
    QWidget* buildConsoleBody()
    {
        logStack = new QStackedWidget;
        console = new QPlainTextEdit;
        console->setObjectName("console");
        console->setReadOnly(true);
        console->setLineWrapMode(QPlainTextEdit::NoWrap);
        console->setMaximumBlockCount(3000);
        logStack->addWidget(console);

        history = new QTableWidget(0, 4);
        styleTable(history);
        history->setHorizontalHeaderLabels({ "#", "PC", "Instr", "Operation" });
        for (int c = 0; c < 4; ++c)
            history->horizontalHeaderItem(c)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        history->verticalHeader()->hide();
        history->verticalHeader()->setDefaultSectionSize(26);
        history->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        history->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        history->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        history->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        history->setEditTriggers(QAbstractItemView::NoEditTriggers);
        history->setAlternatingRowColors(true);
        history->setShowGrid(false);
        history->setSelectionBehavior(QAbstractItemView::SelectRows);
        logStack->addWidget(history);
        return logStack;
    }

    // ------------------------------------------------------------------ UI: buttons
    QWidget* buildButtonBar()
    {
        auto* p = new QFrame;
        p->setObjectName("panel");
        auto* h = new QHBoxLayout(p);
        h->setContentsMargins(14, 12, 14, 12);
        h->setSpacing(12);
        auto mk = [&](Ik k, const QString& t, const char* id, const QString& tip) {
            auto* b = new QPushButton(QStringLiteral(" ") + t);
            b->setObjectName(id);
            const bool white = (QString(id) != "act");
            b->setIcon(mkIcon(k, white ? QColor("#ffffff") : QColor("#e8ecff")));
            b->setIconSize(QSize(22, 22));
            b->setCursor(Qt::PointingHandCursor);
            b->setFocusPolicy(Qt::NoFocus);
            b->setToolTip(tip);
            b->setMinimumHeight(54);
            h->addWidget(b, 1);
            return b;
            };
        btnLoad = mk(IkLoad, "Load", "act", "Load the editor program into memory  (Ctrl+L)");
        btnRun = mk(IkRun, "Run", "run", "Run until HALT  (F5)");
        btnStep = mk(IkStep, "Step", "act", "Execute one instruction  (F10)");
        btnReset = mk(IkReset, "Reset", "act", "Clear registers & CPU state  (Ctrl+Shift+R)");
        btnStop = mk(IkStop, "Stop", "stop", "Stop a running program  (Shift+F5)");
        connect(btnLoad, &QPushButton::clicked, this, [this] { loadProgram(); });
        connect(btnRun, &QPushButton::clicked, this, [this] { onRun(); });
        connect(btnStep, &QPushButton::clicked, this, [this] { onStep(); });
        connect(btnReset, &QPushButton::clicked, this, [this] { onReset(); });
        connect(btnStop, &QPushButton::clicked, this, [this] { onStop(); });
        return p;
    }

    void setupShortcuts()
    {
        auto sc = [this](const QKeySequence& k, std::function<void()> f) {
            auto* a = new QAction(this);
            a->setShortcut(k);
            a->setShortcutContext(Qt::WindowShortcut);
            connect(a, &QAction::triggered, this, [f] { f(); });
            addAction(a);
            };
        sc(QKeySequence(Qt::Key_F5), [this] { if (btnRun->isEnabled()) onRun(); });
        sc(QKeySequence(Qt::Key_F10), [this] { if (btnStep->isEnabled()) onStep(); });
        sc(QKeySequence(Qt::SHIFT | Qt::Key_F5), [this] { onStop(); });
        sc(QKeySequence(Qt::CTRL | Qt::Key_L), [this] { if (btnLoad->isEnabled()) loadProgram(); });
        sc(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R), [this] { onReset(); });
        sc(QKeySequence(Qt::Key_Escape), [this] {
            if (focusKind >= 0) { closeFocus(); navEditor->setChecked(true); lastNav = navEditor; }
            });
    }

    void showSettings()
    {
        QDialog dlg(this);
        dlg.setWindowTitle("Settings");
        dlg.setMinimumWidth(520);
        auto* f = new QFormLayout(&dlg);
        f->setContentsMargins(20, 18, 20, 16);
        f->setVerticalSpacing(12);
        auto* sp = new QSpinBox;
        sp->setRange(0, 2000);
        sp->setValue(delayMs);
        sp->setSuffix(" ms");
        auto* lim = new QSpinBox;
        lim->setRange(1, 10000000);
        lim->setValue(cycleLimit);
        auto* al = new QCheckBox("Auto-load the editor program when pressing Run / Step");
        al->setChecked(autoLoad);
        f->addRow("Delay per instruction  (0 = max speed):", sp);
        f->addRow("Max instructions per Run  (infinite-loop guard):", lim);
        f->addRow(al);
        auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        f->addRow(bb);
        connect(bb, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(bb, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        if (dlg.exec() == QDialog::Accepted) {
            delayMs = sp->value();
            cycleLimit = lim->value();
            autoLoad = al->isChecked();
            if (st == St::Running) runTimer.setInterval(delayMs == 0 ? 1 : delayMs);
            log("Settings updated.", Lg::Info);
        }
    }

    // ------------------------------------------------------------------ expand / collapse a panel
    void layoutRegs(bool big)
    {
        for (int i = 0; i < 16; ++i) {
            regGrid->removeWidget(regLbl[i]);
            regGrid->removeWidget(regEdit[i]);
            regGrid->removeWidget(regDec[i]);
            regGrid->removeWidget(regBin[i]);
            regBin[i]->hide();
        }
        for (int c = 0; c < 8; ++c) { regGrid->setColumnStretch(c, 0); regGrid->setColumnMinimumWidth(c, 0); }
        for (int r = 0; r < 9; ++r) regGrid->setRowStretch(r, big ? 1 : 0);
        regGrid->setVerticalSpacing(big ? 8 : 6);
        for (int i = 0; i < 16; ++i) {
            const int col = i / 8, row = i % 8;
            const QVariant bigV = big;
            for (QWidget* w : { static_cast<QWidget*>(regLbl[i]), static_cast<QWidget*>(regEdit[i]) }) {
                w->setProperty("big", bigV);
                repolish(w);
            }
            if (!big) {
                regGrid->addWidget(regLbl[i], row, col * 2);
                regGrid->addWidget(regEdit[i], row, col * 2 + 1);
                regDec[i]->hide();
            }
            else {
                regGrid->addWidget(regLbl[i], row, col * 3);
                regGrid->addWidget(regEdit[i], row, col * 3 + 1);
                regGrid->addWidget(regDec[i], row, col * 3 + 2);
                regDec[i]->show();
            }
        }
        if (!big) {
            regGrid->setColumnStretch(1, 1);
            regGrid->setColumnStretch(3, 1);
            regGrid->setColumnMinimumWidth(1, 70);
            regGrid->setColumnMinimumWidth(3, 70);
        }
        else {
            for (int c : {1, 4}) { regGrid->setColumnStretch(c, 3); regGrid->setColumnMinimumWidth(c, 70); }
            for (int c : {2, 5}) { regGrid->setColumnStretch(c, 2); regGrid->setColumnMinimumWidth(c, 52); }
        }
    }

    static double lerp(double a, double b, double t) { return a + (b - a) * t; }

    void setResultCompact(bool c)
    {
        resBin->setVisible(!c);
        resAscii->setVisible(!c);
        resHex->setMinimumSize(c ? 92 : 120, 70);
    }

    void setStatusCompact(bool c)
    {
        instrVal->setVisible(!c);
        badge->setProperty("compact", c);
        repolish(badge);
        badge->setMinimumHeight(c ? 34 : 52);
        if (c) { statusGrid->setContentsMargins(20, 6, 16, 8); statusGrid->setVerticalSpacing(2); }
        else { statusGrid->setContentsMargins(22, 12, 18, 16); statusGrid->setVerticalSpacing(8); }
    }

    // t = 0 -> normal dashboard, t = 1 -> expanded
    void applyExpand(int kind, double t)
    {
        if (kind == 0) {
            statusPanel->setMaximumHeight(int(lerp(expStatusH, expStatusCompactH, t)));
            regPanel->setMinimumHeight(int(lerp(expRegH, expRegTargetH, t)));
            const int eh = int(lerp(expEditH, 46, t));
            for (QLineEdit* e : regEdit) e->setMaximumHeight(eh);
        }
        else if (kind == 1) {
            gridLay->setColumnStretch(0, int(lerp(900, 1800, t)));
            gridLay->setColumnStretch(1, int(lerp(1000, 200, t)));
        }
    }

    void prepareExpand(int kind)
    {
        if (kind == 0) {
            expStatusH = statusPanel->height();
            expRegH = regPanel->height();
            expEditH = qMax(30, regEdit[0]->height());
            setStatusCompact(true);
            statusPanel->setMaximumHeight(QWIDGETSIZE_MAX);
            statusPanel->layout()->activate();
            expStatusCompactH = statusPanel->layout()->minimumSize().height();
            statusPanel->setMaximumHeight(expStatusH);
            expRegTargetH = qMax(expRegH, rightCol->height() - expStatusCompactH - rightLay->spacing());
            layoutRegs(true);
        }
        else if (kind == 1) {
            setResultCompact(true);
            gridLay->setColumnMinimumWidth(2, rightCol->width());   // registers column keeps its width
        }
    }

    void startAnim(double to)
    {
        expTarget = to > 0.5 ? 1 : 0;
        expAnim->stop();
        expAnim->setStartValue(curT);
        expAnim->setEndValue(to);
        expAnim->start();
    }

    void finishCollapse()
    {
        if (focusKind == 0) {
            regPanel->setMinimumHeight(0);
            statusPanel->setMaximumHeight(QWIDGETSIZE_MAX);
            for (QLineEdit* e : regEdit) e->setMaximumHeight(QWIDGETSIZE_MAX);
            rightLay->setStretch(0, 0);
            rightLay->setStretch(2, 1);
            setStatusCompact(false);
            layoutRegs(false);
        }
        else if (focusKind == 1) {
            gridLay->setColumnStretch(0, 9);
            gridLay->setColumnStretch(1, 10);
            gridLay->setColumnMinimumWidth(2, 0);
            setResultCompact(false);
        }
        focusKind = -1;
        curT = 0.0;
        refreshAll(false);
    }

    void onExpandDone()
    {
        if (focusKind < 0) return;
        if (expTarget == 0) { finishCollapse(); return; }
        if (focusKind == 0) {
            regPanel->setMinimumHeight(0);
            rightLay->setStretch(0, 1);          // registers keep the freed space when the window resizes
            rightLay->setStretch(2, 0);
        }
        else {
            memBig = true;
            memTable->setColumnHidden(3, false);
            {
                QHeaderView* hh = memTable->horizontalHeader();
                const int w[3] = { 84, 96, 64 };
                for (int c = 0; c < 3; ++c) { hh->setSectionResizeMode(c, QHeaderView::Fixed); hh->resizeSection(c, w[c]); }
                hh->setSectionResizeMode(3, QHeaderView::Stretch);
            }
            refreshAll(false);
        }
    }

    void toggleFocus(int kind)
    {
        if (focusKind == kind && expTarget == 1) {
            closeFocus();
            navEditor->setChecked(true);
            lastNav = navEditor;
        }
        else {
            openFocus(kind);
        }
    }

    void openFocus(int kind)
    {
        if (focusKind >= 0 && focusKind != kind) closeFocus(true);
        if (focusKind < 0) {
            focusKind = kind;
            curT = 0.0;
            prepareExpand(kind);
        }
        startAnim(1.0);
    }

    void closeFocus(bool instant = false)
    {
        if (focusKind < 0) return;
        expAnim->stop();
        if (focusKind == 1) {
            memBig = false;
            memTable->setColumnHidden(3, true);
            memTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        }
        if (focusKind == 0) {
            if (curT >= 0.99) {                  // re-measure: the window may have been resized
                expRegTargetH = regPanel->height();
                expStatusCompactH = statusPanel->height();
            }
            rightLay->setStretch(0, 0);
            rightLay->setStretch(2, 1);
            regPanel->setMinimumHeight(regPanel->height());
        }
        if (instant) {
            applyExpand(focusKind, 0.0);
            finishCollapse();
            return;
        }
        startAnim(0.0);
    }

    // ------------------------------------------------------------------ result helpers
    void captureSnapshot()
    {
        for (int i = 0; i < 16; ++i) regStart[i] = RegesterFile::Read(i);
        for (int i = 0; i < 256; ++i) {
            const QString raw = QString::fromStdString(Memory::Read(i)).toUpper();
            memStart[i] = raw.isEmpty() ? QStringLiteral("00") : raw;
        }
    }

    QStringList changedRegs() const
    {
        QStringList l;
        for (int i = 0; i < 16; ++i) {
            const int v = RegesterFile::Read(i);
            if (v != regStart[i]) l << QString("R%1=%2").arg(hx(i, 1), hx(v));
        }
        return l;
    }

    QStringList changedMem() const
    {
        QStringList l;
        for (int i = 0; i < 256; ++i) {
            const QString raw = QString::fromStdString(Memory::Read(i)).toUpper();
            const QString shown = raw.isEmpty() ? QStringLiteral("00") : raw;
            if (shown != memStart[i]) l << QString("[%1]=%2").arg(hx(i), shown);
        }
        return l;
    }

    static QString joinLimited(const QStringList& l, int maxN = 10)
    {
        if (l.isEmpty()) return QStringLiteral("no changes");
        QStringList part = l.mid(0, maxN);
        QString r = part.join("   ");
        if (l.size() > maxN) r += QString("   ... (+%1 more)").arg(l.size() - maxN);
        return r;
    }

    // ------------------------------------------------------------------ machine control
    bool loadProgram()
    {
        QString err;
        QStringList warns;
        const std::vector<std::string> codes = parseProgram(curEditor()->toPlainText(), err, warns);
        if (!err.isEmpty()) {
            log(err, Lg::Err);
            return false;
        }
        if (codes.empty()) {
            log("The editor is empty - nothing to load.", Lg::Warn);
            return false;
        }
        const int sa = startAddr();
        try {
            // use YOUR loader: VoleMechine::PropramLoader(path, startAddress)
            QTemporaryFile tmp(QDir::temp().filePath("vole_XXXXXX.txt"));
            tmp.setAutoRemove(true);
            if (!tmp.open()) { log("Could not create a temporary program file.", Lg::Err); return false; }
            tmp.write(curEditor()->toPlainText().toUtf8());
            tmp.flush();
            std::string path = tmp.fileName().toStdString();
            int startingAddress = sa;
            if (!VoleMechine::PropramLoader(path, startingAddress)) {
                log("Failed to Load Program! File Not Found.", Lg::Err);
                return false;
            }
        }
        catch (const std::exception& e) {
            log(QString("Load failed: %1").arg(e.what()), Lg::Err);
            return false;
        }
        for (const QString& w : warns) log(w, Lg::Warn);

        CPU::SetHalted(false);
        CPU::SetPC(sa);
        CPU::setStartingAddress(sa);
        try { CPU::SetIR(sa); }
        catch (...) {}
        executing = false;
        dirty = false;
        cycles = 0;
        setState(St::Ready);
        log(QString("Program loaded successfully (%1 bytes at address %2).").arg(codes.size()).arg(hx(sa)), Lg::Ok);
        refreshAll(true);
        return true;
    }

    bool beginExecution()
    {
        if (executing) return true;
        if (st == St::Halted || st == St::Faulted) setState(St::Ready);
        bool startEmpty = false;
        try { startEmpty = Memory::Read(startAddr()).empty(); }
        catch (...) {}
        if (autoLoad && (dirty || startEmpty) && !loadProgram()) return false;

        const int sa = startAddr();
        CPU::SetHalted(false);
        CPU::SetPC(sa);
        CPU::setStartingAddress(sa);
        MemoryEdit e;
        e.Address = 255;
        e.Value = "";
        VoleMechine::WriteMemory(e);                  // same as CPU::Run(): clear the screen cell
        captureSnapshot();
        cycles = 0;
        pending.clear();
        history->setRowCount(0);
        executing = true;
        return true;
    }

    void onRun()
    {
        if (st == St::Running) return;
        if (!beginExecution()) return;
        budgetStart = cycles;
        setState(St::Running);
        log("Starting execution...", Lg::Info);
        runTimer.start(delayMs == 0 ? 1 : delayMs);
    }

    void onStep()
    {
        if (st == St::Running) return;
        if (!beginExecution()) return;
        budgetStart = cycles;
        stepOnce(true);
        if (st != St::Halted && st != St::Faulted) setState(St::Paused);
        refreshAll(true);
    }

    void onStop()
    {
        if (st != St::Running) return;
        setState(St::Paused);
        log("Execution stopped by user.", Lg::Warn);
        refreshAll(true);
    }

    void onReset()
    {
        runTimer.stop();
        RegesterFile::Reset();
        MemoryEdit e;
        e.Address = 255;
        e.Value = "";
        VoleMechine::WriteMemory(e);
        const int sa = startAddr();
        CPU::SetHalted(false);
        CPU::SetPC(sa);
        try { CPU::SetIR(sa); }
        catch (...) {}
        cycles = 0;
        executing = false;
        pending.clear();
        history->setRowCount(0);
        setState(St::Ready);
        log(QString("CPU reset  (registers cleared, PC = %1).").arg(hx(sa)), Lg::Info);
        refreshAll(true);
    }

    void onTick()
    {
        const int n = (delayMs == 0) ? 500 : 1;
        for (int i = 0; i < n && st == St::Running; ++i) {
            if (!stepOnce(false)) break;
            if (cycles - budgetStart >= cycleLimit) {
                setState(St::Paused);
                log(QString("Paused: %1 instructions executed without HALT (possible infinite loop).").arg(cycleLimit), Lg::Warn);
                break;
            }
        }
        refreshAll(true);
    }

    void logFinalResult()
    {
        const QString sc = QString::fromStdString(Memory::Read(255)).trimmed().toUpper();
        bool ok = false;
        const int b = sc.toInt(&ok, 16);
        if (sc.isEmpty() || !ok) {
            log("Result:  nothing was stored at the Screen (address FF).", Lg::Warn);
        }
        else {
            QString txt = QString("Result:  Screen (FF) = %1   (decimal %2").arg(hx(b)).arg(b);
            if (b >= 32 && b < 127) txt += QString(", '%1'").arg(QChar(b));
            txt += ")";
            log(txt, Lg::Ok);
        }
        log("Registers changed:  " + joinLimited(changedRegs()), Lg::Info);
        log("Memory changed:  " + joinLimited(changedMem()), Lg::Info);
    }

    void fault(const QString& msg)
    {
        setState(St::Faulted);
        log(QString("Status: Faulted.  Executed instructions: %1").arg(cycles), Lg::Err);
        log("Error: " + msg, Lg::Err);
    }

    // Executes ONE instruction using exactly the same sequence as CPU::Run().
    bool stepOnce(bool verbose)
    {
        if (st == St::Halted || st == St::Faulted) return false;
        const int pcBefore = CPU::GetPC();
        try {
            CPU::SetIR(pcBefore);
            CPU::IncrementPC();
            Instruction::Decode();
            const ValidationResult v = Instruction::Validate();
            if (!v.IsValid) {
                fault(QString("%1  (instruction %2 at address %3)")
                    .arg(QString::fromStdString(v.ErrorMessage),
                        QString::fromStdString(CPU::GetIR()).toUpper(), hx(pcBefore)));
                return false;
            }
            const QString ir = QString::fromStdString(CPU::GetIR()).toUpper();
            Instruction::Execute();
            ++cycles;
            pending.push_back({ cycles, pcBefore, ir, disasm(ir) });
            if (pending.size() > 300) pending.removeFirst();
            if (verbose) log(QString("[%1]  %2   %3").arg(hx(pcBefore), ir, disasm(ir)), Lg::Info);
            if (CPU::GetHalted()) {
                setState(St::Halted);
                log("Execution finished.", Lg::Info);
                log("CPU is halted.", Lg::Ok);
                logFinalResult();
                return false;
            }
        }
        catch (const std::exception& e) {
            bool empty = false;
            try { empty = Memory::Read(pcBefore).empty() || (pcBefore + 1 < 256 && Memory::Read(pcBefore + 1).empty()); }
            catch (...) {}
            if (empty)
                fault(QString("Memory at address %1 is empty - press Load to put your program in memory first.").arg(hx(pcBefore)));
            else
                fault(QString("Cannot execute the instruction at address %1 - invalid data or out of range.  (%2)")
                    .arg(hx(pcBefore), e.what()));
            return false;
        }
        return true;
    }

    void setState(St s)
    {
        st = s;
        if (s != St::Running) runTimer.stop();
        if (s == St::Halted || s == St::Faulted) executing = false;
        static const char* ids[] = { "ready", "running", "paused", "halted", "faulted" };
        static const char* txt[] = { "Ready", "Running", "Paused", "Halted", "Faulted" };
        const int i = static_cast<int>(s);
        badge->setText(QString::fromUtf8("\u25CF  ") + txt[i]);
        badge->setProperty("state", ids[i]);
        repolish(badge);
        const bool run = (s == St::Running);
        btnRun->setEnabled(!run);
        btnStep->setEnabled(!run);
        btnLoad->setEnabled(!run);
        btnStop->setEnabled(run);
        for (auto* e : regEdit) e->setReadOnly(run);
    }

    // ------------------------------------------------------------------ view refresh
    void refreshAll(bool follow)
    {
        if (st == St::Ready && !executing) captureSnapshot();
        // registers
        for (int i = 0; i < 16; ++i) {
            const int v = RegesterFile::Read(i);
            QLineEdit* e = regEdit[i];
            const QString t = hx(v);
            if (!e->hasFocus() || e->isReadOnly()) {
                if (e->text() != t) e->setText(t);
            }
            const bool ch = (st == St::Running || st == St::Paused) && v != regCache[i];
            if (e->property("changed").toBool() != ch) {
                e->setProperty("changed", ch);
                repolish(e);
            }
            regCache[i] = v;
            regDec[i]->setText(QString("= %1").arg(v));
            {
                const QString bn = QString::number(v & 0xFF, 2).rightJustified(8, '0');
                regBin[i]->setText(bn.left(4) + " " + bn.right(4));
            }
            e->setToolTip(QString("R%1 = %2 (decimal)").arg(hx(i, 1)).arg(v));
        }

        // cpu status
        const int pc = CPU::GetPC();
        const QString ir = QString::fromStdString(CPU::GetIR()).toUpper();
        pcVal->setText(hx(pc));
        irVal->setText(ir.isEmpty() ? QStringLiteral("----") : ir);
        cycVal->setText(QString::number(cycles));
        instrVal->setText(ir.isEmpty() ? QString() : disasm(ir));

        // memory  (model/view: only changed rows are repainted)
        // highlight is active while executing and after HALT (so the last write before HALT is shown)
        const bool hlOn = (st == St::Running || st == St::Paused || st == St::Halted);
        int wr = -1;
        {
            QString nv[256], nd[256];
            bool ne[256];
            for (int i = 0; i < 256; ++i) {
                const QString raw = QString::fromStdString(Memory::Read(i)).toUpper();
                ne[i] = raw.isEmpty();
                nv[i] = ne[i] ? QStringLiteral("00") : raw;
            }
            if (memBig) {
                const int sa = startAddr();
                for (int i = 0; i + 1 < 256; ++i) {
                    if (nv[i] == "00" || (((i - sa) & 1) != 0)) continue;
                    const QString d = disasm(nv[i] + nv[i + 1]);
                    if (d != "???" && d != "-") nd[i] = d;
                }
            }
            wr = memModel->setState(nv, ne, nd, -1, hlOn);
        }
        if (follow) {
            if (wr >= 0)                                   // scroll to the cell that was just written
                memTable->scrollTo(memModel->index(wr, 0), QAbstractItemView::EnsureVisible);
            else if (!memModel->flashing() && pc >= 0 && pc <= 255)
                memTable->scrollTo(memModel->index(pc, 0), QAbstractItemView::EnsureVisible);
        }

        // final result (memory[255] = Screen) + what the program changed
        const QString sc = QString::fromStdString(Memory::Read(255)).trimmed().toUpper();
        bool ok = false;
        const int b = sc.toInt(&ok, 16);
        const bool has = !sc.isEmpty() && ok;
        resHex->setText(has ? hx(b) : QStringLiteral("--"));
        resDec->setText(has ? QString("Decimal :  %1").arg(b) : QStringLiteral("Decimal :  -"));
        resBin->setText(has ? QString("Binary  :  %1").arg(QString::number(b & 0xFF, 2).rightJustified(8, '0'))
            : QStringLiteral("Binary  :  -"));
        resAscii->setText(has ? QString("ASCII   :  %1").arg((b >= 32 && b < 127) ? QString("'%1'").arg(QChar(b)) : QStringLiteral("-"))
            : QStringLiteral("ASCII   :  -"));
        resRegs->setText("Registers changed:  " + joinLimited(changedRegs(), 8));
        resMem->setText("Memory changed:  " + joinLimited(changedMem(), 8));

        // history
        if (!pending.isEmpty()) {
            history->setUpdatesEnabled(false);
            if (pending.size() + history->rowCount() > 1000) history->setRowCount(0);
            for (const Hist& h : pending) {
                const int r = history->rowCount();
                history->insertRow(r);
                auto cell = [](const QString& t, const QColor& c) {
                    auto* i = new QTableWidgetItem(t);
                    i->setForeground(c);
                    return i;
                    };
                history->setItem(r, 0, cell(QString::number(h.n), QColor("#8e98c9")));
                history->setItem(r, 1, cell(hx(h.pc), QColor("#35a0ff")));
                history->setItem(r, 2, cell(h.ir, QColor("#25e07b")));
                history->setItem(r, 3, cell(h.desc, QColor("#e8ecff")));
            }
            while (history->rowCount() > 1000) history->removeRow(0);
            pending.clear();
            history->setUpdatesEnabled(true);
            history->scrollToBottom();
        }
    }

    // ------------------------------------------------------------------ style
    static QString styleSheet_()
    {
        return QString::fromUtf8(R"QSS(
* { font-family: "Segoe UI", "Inter", "Noto Sans", sans-serif; color: #e8ecff; font-size: 13px; }
QWidget#root { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #080918, stop:0.6 #0b0d2a, stop:1 #100f45); }
QWidget#titleBar { background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #090a20, stop:0.55 #1a1f7a, stop:1 #090a20); }
QLabel#logo { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #3d5afe, stop:1 #8a5cff); border-radius: 11px; font-size: 24px; font-weight: 800; color: white; }
QLabel#appName { font-size: 19px; font-weight: 700; }
QLabel#appSub { color: #98a3d8; font-size: 14px; }
QFrame#vsep { background: #3a3f8f; border: none; }
QLabel#dim { color: #8e98c9; }
QPushButton#winBtn, QPushButton#closeBtn { background: transparent; border: none; border-radius: 0px; padding: 0px; }
QPushButton#winBtn:hover { background: rgba(255,255,255,0.10); }
QPushButton#closeBtn:hover { background: #e5173f; color: white; }

QFrame#sidebar { background: rgba(13,15,46,0.92); border: 1px solid #23275e; border-radius: 16px; }
QPushButton#nav { text-align: left; padding: 8px 12px 8px 16px; border: none; border-radius: 12px; background: transparent; color: #b7c0ec; font-size: 14px; }
QPushButton#nav:hover { background: rgba(122,92,255,0.16); }
QPushButton#nav:checked { background: qlineargradient(x1:0,y1:0,x2:1,y2:0, stop:0 #4b3cf0, stop:1 #7a5cff); color: white; }
QLabel#watermark { color: rgba(110,120,255,0.20); font-size: 110px; font-weight: 800; }

QFrame#panel { background: #101233; border: 1px solid #2a2e75; border-radius: 16px; }
QFrame#panel[flash="true"] { border: 1px solid #8a6cff; background: #14173f; }
QWidget#panelHeader { background: transparent; }
QLabel#panelTitle { font-size: 16px; font-weight: 700; }
QLabel#panelIcon { color: #6d8bff; font-size: 20px; }
QFrame#hline { background: #23275e; border: none; }
QFrame#footer { background: transparent; border: none; border-top: 1px solid #23275e; }
QToolButton#chev { border: none; background: transparent; }

QPlainTextEdit#code { background: #0b0c22; border: none; color: #e8ecff; font-family: Consolas, "Cascadia Mono", "DejaVu Sans Mono", monospace; font-size: 19px; selection-background-color: #3a3fa8; }
QPlainTextEdit#console { background: #0a0b22; border: none; color: #e8ecff; font-family: Consolas, "Cascadia Mono", "DejaVu Sans Mono", monospace; font-size: 14px; padding: 8px; }

QTabBar::tab { background: #0f1130; border: 1px solid #2a2e75; border-bottom: none; padding: 9px 12px 9px 18px; border-top-left-radius: 10px; border-top-right-radius: 10px; margin-right: 4px; color: #aeb7e6; font-size: 14px; }
QTabBar::tab:selected { background: #171a52; color: white; border-color: #4a45d8; }
QToolButton#tbtn { background: #14173f; border: 1px solid #2c3182; border-radius: 8px; padding: 6px 12px; font-size: 14px; }
QToolButton#tbtn:hover { background: #1b1f58; border-color: #5a55ff; }

QLineEdit#reg { background: #171a40; border: 1px solid #23275e; border-radius: 8px; padding: 2px 6px; font-family: Consolas, "Cascadia Mono", "DejaVu Sans Mono", monospace; font-size: 16px; color: #e8ecff; }
QLineEdit#reg:focus { border-color: #7a5cff; }
QLineEdit#reg[changed="true"] { color: #35a0ff; border-color: #35a0ff; font-weight: bold; }
QLineEdit#reg[big="true"] { font-size: 19px; border-radius: 10px; padding: 2px 8px; }
QLabel#regLabel[big="true"] { font-size: 16px; min-width: 30px; }
QLabel#regDec { color: #aeb7e6; font-family: Consolas, "Cascadia Mono", "DejaVu Sans Mono", monospace; font-size: 15px; }
QLabel#regBin { color: #6d8bff; font-family: Consolas, "Cascadia Mono", "DejaVu Sans Mono", monospace; font-size: 17px; }
QTableView#memTable[big="true"] { font-size: 19px; }
QLabel#badge[compact="true"] { font-size: 16px; padding: 4px 12px; border-radius: 12px; }
QLabel#regLabel { color: #c9d0ff; font-size: 15px; font-weight: 600; min-width: 28px; }
QLineEdit#startEdit { background: #171a40; border: 1px solid #2a2e75; border-radius: 8px; padding: 4px 6px; font-family: Consolas, monospace; font-size: 14px; }
QLineEdit#startEdit:focus { border-color: #7a5cff; }

QLabel#statName { color: #8e98c9; font-size: 15px; }
QLabel#pcVal { color: #35a0ff; font-family: Consolas, "DejaVu Sans Mono", monospace; font-size: 17px; font-weight: 600; }
QLabel#irVal { color: #25e07b; font-family: Consolas, "DejaVu Sans Mono", monospace; font-size: 17px; font-weight: 600; }
QLabel#cycVal { color: #b07bff; font-family: Consolas, "DejaVu Sans Mono", monospace; font-size: 17px; font-weight: 600; }
QLabel#instrVal { color: #8e98c9; font-family: Consolas, "DejaVu Sans Mono", monospace; font-size: 14px; }
QLabel#badge { border-radius: 16px; padding: 8px 18px; font-size: 20px; font-weight: 700; border: 1px solid #1f7a55; background: #0f3a2c; color: #3dffa0; }
QLabel#badge[state="running"] { background: #0f2c4d; border-color: #2b78c9; color: #5cb8ff; }
QLabel#badge[state="paused"]  { background: #3d2e0b; border-color: #c28a1a; color: #ffcf5a; }
QLabel#badge[state="halted"]  { background: #2a1a52; border-color: #7a5cff; color: #c3b0ff; }
QLabel#badge[state="faulted"] { background: #4a0f1d; border-color: #c4203f; color: #ff6b85; }

QLabel#resHex { background: #05060f; border: 1px solid #1f8a5a; border-radius: 12px; color: #3dffa0; font-family: Consolas, "DejaVu Sans Mono", monospace; font-size: 38px; font-weight: 800; }
QLabel#resLine { color: #e8ecff; font-family: Consolas, "DejaVu Sans Mono", monospace; font-size: 15px; }
QLabel#resNote { color: #aeb7e6; font-size: 13px; }
QToolButton#tabX { border: none; background: transparent; border-radius: 6px; padding: 0px; margin: 0px 6px 0px 4px; }
QToolButton#tabX:hover { background: #2a2e75; }
QLabel#screenOLD { background: #05060f; border: 1px solid #1f5a3a; border-radius: 10px; color: #3dffa0; font-family: Consolas, "DejaVu Sans Mono", monospace; font-size: 24px; font-weight: 700; padding: 4px 14px; }
QLabel#screenAscii { color: #ffd24a; font-family: Consolas, "DejaVu Sans Mono", monospace; font-size: 22px; font-weight: 700; }

QTableView { font-family: Consolas, "Cascadia Mono", "DejaVu Sans Mono", monospace; font-size: 14px; }

QPushButton { background: #1a1d52; border: 1px solid #343aa0; border-radius: 8px; padding: 6px 16px; }
QPushButton:hover { background: #22267a; }
QPushButton#small { padding: 5px 14px; }
QPushButton#act, QPushButton#run, QPushButton#stop { min-height: 54px; border-radius: 14px; font-size: 17px; font-weight: 600; }
QPushButton#act { background: #14173f; border: 1px solid #2c3182; }
QPushButton#act:hover { background: #1b1f58; border-color: #5a55ff; }
QPushButton#run { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #5b3df5, stop:1 #8a5cff); border: 1px solid #9a86ff; color: white; }
QPushButton#run:hover { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #6c50ff, stop:1 #9a74ff); }
QPushButton#stop { background: qlineargradient(x1:0,y1:0,x2:1,y2:1, stop:0 #8f1330, stop:1 #c4203f); border: 1px solid #ff5272; color: white; }
QPushButton#act:disabled, QPushButton#run:disabled, QPushButton#stop:disabled { background: #0f1131; border: 1px solid #1d2050; color: #565e8f; }

QScrollBar:vertical { background: transparent; width: 10px; margin: 2px; }
QScrollBar::handle:vertical { background: #2f3485; border-radius: 4px; min-height: 30px; }
QScrollBar::handle:vertical:hover { background: #4a50c8; }
QScrollBar:horizontal { background: transparent; height: 10px; margin: 2px; }
QScrollBar::handle:horizontal { background: #2f3485; border-radius: 4px; min-width: 30px; }
QScrollBar::add-line, QScrollBar::sub-line { width: 0px; height: 0px; }
QScrollBar::add-page, QScrollBar::sub-page { background: transparent; }

QDialog, QMessageBox { background: #0d0f2b; }
QSpinBox { background: #171a40; border: 1px solid #2a2e75; border-radius: 8px; padding: 5px 8px; }
QCheckBox::indicator { width: 16px; height: 16px; }
QToolTip { background: #14173f; color: #e8ecff; border: 1px solid #3a3fa8; padding: 4px; }
)QSS");
    }

    // keep QSS helper from being shadowed by the QWidget::styleSheet() getter
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    VoleWindow w;
    w.show();
    return app.exec();
}