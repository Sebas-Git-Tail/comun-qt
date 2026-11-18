// Pruebas de ui/: recordarVentana guarda el tamaño y si la ventana estaba maximizada o en pantalla completa.
#include "Piezas.h"

#include <QApplication>
#include <QSettings>
#include <QTemporaryDir>
#include <QtTest>

class PruebaUi : public QObject
{
    Q_OBJECT

    QTemporaryDir dir_;

    QString archivo(const char *nombre) const { return dir_.filePath(QString::fromLatin1(nombre) + QStringLiteral(".ini")); }

private slots:
    void recuerdaElTamano()
    {
        QSettings ajustes(archivo("tamano"), QSettings::IniFormat);
        {
            QWidget v;
            tema::recordarVentana(&v, &ajustes);
            v.resize(731, 517);
            v.show();
            QVERIFY(QTest::qWaitForWindowExposed(&v));
            v.close();
        }
        QVERIFY(ajustes.contains(QStringLiteral("ventana/geometria")));
        QWidget otra;
        otra.resize(300, 200);
        tema::recordarVentana(&otra, &ajustes);
        otra.show();
        QVERIFY(QTest::qWaitForWindowExposed(&otra));
        QCOMPARE(otra.size(), QSize(731, 517));
        QVERIFY(!otra.isMaximized());
    }

    void recuerdaMaximizadaYPantallaCompleta()
    {
        for (const Qt::WindowState estado : {Qt::WindowMaximized, Qt::WindowFullScreen}) {
            QSettings ajustes(archivo(estado == Qt::WindowMaximized ? "max" : "completa"), QSettings::IniFormat);
            {
                QWidget v;
                tema::recordarVentana(&v, &ajustes);
                v.resize(640, 480);
                v.show();
                QVERIFY(QTest::qWaitForWindowExposed(&v));
                v.setWindowState(estado);
                QTRY_VERIFY(v.windowState() & estado);
                v.close();
            }
            QWidget otra;
            tema::recordarVentana(&otra, &ajustes);
            otra.show();
            QVERIFY(QTest::qWaitForWindowExposed(&otra));
            QTRY_VERIFY(otra.windowState() & estado);
            // Al salir de maximizada / pantalla completa vuelve al tamaño de antes
            otra.setWindowState(Qt::WindowNoState);
            QTRY_COMPARE(otra.size(), QSize(640, 480));
        }
    }

    void guardaAlSalirDeLaApp()
    {
        QSettings ajustes(archivo("salir"), QSettings::IniFormat);
        QWidget v;
        tema::recordarVentana(&v, &ajustes);
        v.resize(555, 444);
        v.show();
        QVERIFY(QTest::qWaitForWindowExposed(&v));
        QVERIFY(!ajustes.contains(QStringLiteral("ventana/geometria")));
        QVERIFY(QMetaObject::invokeMethod(qApp, "aboutToQuit", Qt::DirectConnection));   // como al salir de la app
        QVERIFY(ajustes.contains(QStringLiteral("ventana/geometria")));
    }
};

QTEST_MAIN(PruebaUi)
#include "tst_ui.moc"
