#ifndef GyverButton_h
#define GyverButton_h
#include <Arduino.h>
#define LIBRARY_VERSION	2.8

/*
	Текущая версия: 2.8 от 17.10.2018
	GyverButton - библиотека для многофункциональной отработки нажатия кнопки. Возможности:
	- Работа с нормально замкнутыми и нормально разомкнутыми кнопками
	- Работа с подключением PULL_UP и PULL_DOWN
	- Опрос кнопки с программным антидребезгом контактов
	- Настраиваемое время антидребезга
	- Отработка нажатия, удерживания, отпускания, клика по кнопке
	- Настраиваемый таймаут удержания
	- Отработка одиночного, двойного и тройного нажатия (вынесено отдельно)
	- Отработка любого количества нажатий кнопки (функция возвращает количество нажатий)
	- Настраиваемый таймаут повторного нажатия/удержания
	- Функция изменения значения переменной с заданным шагом и заданным интервалом по времени
	- Возможность опрашивать не кнопку, а напрямую давать величину (все возможности библиотеки для матричных и резистивных клавиатур)
*/

#pragma pack(push,1)
typedef struct
{	
	bool btn_state: 1;
	bool btn_flag: 1;
	bool hold_flag: 1;
	bool counter_flag: 1;
	bool isHolded_f: 1;
	bool isRelease_f: 1;
	bool isPress_f: 1;
	bool step_flag: 1;
	bool oneClick_f: 1;
	bool isOne_f: 1;

} GyverButtonFlags;
#pragma pack(pop)

class GButton
{
  public:
    GButton(uint8_t pin);								// класс кнопки, принимает пин
	
	GButton(uint8_t pin, bool type, bool dir);	// класс кнопки, принимает PIN пин, тип type (HIGH_PULL / LOW_PULL) и направление dir (NORM_OPEN / NORM_CLOSE)
														// HIGH_PULL - кнопка подключена к GND, пин подтянут к VCC, pinMode - INPUT_PULLUP (по умолчанию)
														// LOW_PULL - кнопка подключена к VCC, пин подтянут к GND, pinMode - INPUT
														// NORM_OPEN - кнопка по умолчанию разомкнута (по умолчанию)
														// NORM_CLOSE - кнопка по умолчанию замкнута
	
	void setDebounce(uint16_t debounce);				// установка времени антидребезга (по умолчанию 80 мс)
	void setTimeout(uint16_t timeout);					// установка таймаута удержания (по умолчанию 500 мс)	
	void setStepTimeout(uint16_t step_timeout);			// установка таймаута между инкрементами (по умолчанию 400 мс)	
	void setType(bool type);							// установка типа кнопки (HIGH_PULL - подтянута к питанию, LOW_PULL - к gnd)	
	void setDirection(bool dir);						// установка направления (разомкнута/замкнута по умолчанию - NORM_OPEN, NORM_CLOSE)	
	
	void setTickMode(bool tickMode);					// (MANUAL / AUTO) ручной или автоматический опрос кнопки функцией tick()	
														// MANUAL - нужно вызывать функцию tick() вручную														
														// AUTO - tick() входит во все остальные функции и опрашивается сама
	
	void tick();										// опрос кнопки	
	void tick(bool state);							// опрос внешнего значения (0 нажато, 1 не нажато) (для матричных, резистивных клавиатур и джойстиков)
	
	bool isPress();		// возвращает true при нажатии на кнопку. Сбрасывается после вызова
	bool isRelease();	// возвращает true при отпускании кнопки. Сбрасывается после вызова
	bool isClick();		// возвращает true при клике. Сбрасывается после вызова
    bool isHolded();		// возвращает true при удержании дольше timeout. Сбрасывается после вызова
	bool isHold();		// возвращает true при нажатой кнопке, не сбрасывается
	bool state();		// возвращает состояние кнопки

	bool isSingle();		// возвращает true при одиночном клике. Сбрасывается после вызова
	bool isDouble();		// возвращает true при двойном клике. Сбрасывается после вызова
	bool isTriple();		// возвращает true при тройном клике. Сбрасывается после вызова
	
	bool hasClicks();	// проверка на наличие кликов. Сбрасывается после вызова
	uint8_t getClicks();	// вернуть количество кликов
	
	bool isStep();		// возвращает true по таймеру setStepTimeout, смотри пример
	
  private:
	void init();
    GyverButtonFlags flags;
    uint8_t _PIN = 0;
	uint16_t _debounce = 0;
	uint16_t _timeout = 0;
	uint16_t _step_timeout = 0;
	uint8_t btn_counter = 0, last_counter = 0;	
	uint32_t btn_timer = 0;		
	bool _inv_state = false;
	bool _mode = false;
	bool _type = false;
	bool _tickMode = false;
};

#define HIGH_PULL 0
#define LOW_PULL 1
#define NORM_OPEN 0
#define NORM_CLOSE 1
#define MANUAL 0
#define AUTO 1
 
#endif