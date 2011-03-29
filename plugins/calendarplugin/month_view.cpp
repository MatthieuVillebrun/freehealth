#include <QPainter>

#include "month_view.h"
#include "week_view.h" // TMP

using namespace Calendar;

MonthView::MonthView(QWidget *parent) : View(parent) {
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

int MonthView::topHeaderHeight() const {
	return 0;
}

int MonthView::leftHeaderWidth() const {
	return 0;
}

QWidget *MonthView::createHeaderWidget(QWidget *parent) {
	WeekHeader *widget = new WeekHeader(parent);
	widget->setFirstDate(m_firstDate);
	return widget;
}

void MonthView::paintBody(QPainter *painter, const QRect &visibleRect) {
	painter->fillRect(visibleRect, Qt::white);
	QPen pen = painter->pen();
	pen.setColor(QColor(200, 200, 200));
	pen.setCapStyle(Qt::FlatCap);
	painter->setPen(pen);

	int horiAmount = visibleRect.width() - 6; // total width without lines width
	int vertiAmount = visibleRect.height() - 4; // total height without lines height

	// vertical lines
	for (int i = 1; i < 7; ++i)
		painter->drawLine((i * horiAmount) / 7 + i - 1, 0,
						  (i * horiAmount) / 7 + i - 1, visibleRect.height());

	// horizontal lines
	for (int i = 1; i < 5; ++i)
		painter->drawLine(0, (i * vertiAmount) / 5 + i - 1,
						  visibleRect.width(), (i * vertiAmount) / 5 + i - 1);

	// get the very first day of the grid (not necessarily the first day of the month)
	QDate firstDay = m_firstDate.addDays(-m_firstDate.dayOfWeek() + 1);

	// day texts
	for (int j = 0; j < 5; ++j)
		for (int i = 0; i < 7; ++i) {
			QRect r((i * horiAmount) / 7 + i, (j * vertiAmount) / 5 + j + 2, // +2 is a correction to be not stucked to the top line
					((i + 1) * horiAmount) / 7 - (i * horiAmount) / 7 - 2, // -2 is a correction to be not stucked to the right line
					((j + 1) * vertiAmount) / 5 - (j * vertiAmount) / 5);

			QString text;
			if (firstDay.day() == 1)
				text = firstDay.toString(tr("d MMM"));
			else
				text = firstDay.toString(tr("d"));

			if (firstDay.month() != m_firstDate.month())
				pen.setColor(QColor(200, 200, 200));
			else
				pen.setColor(QColor(150, 150, 150));
			painter->setPen(pen);

			painter->drawText(r, Qt::AlignRight | Qt::AlignTop, text);
			firstDay = firstDay.addDays(1);
		}
}
