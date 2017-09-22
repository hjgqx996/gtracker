#include "MMA8451.h"
#include "I2C2.h"

static MMA8451_TDataState deviceData;

uint8_t I2C_Read(uint8_t addr, uint8_t *data) {

	I2C_ReadBuffer(addr,data,1);
}
//--------------------------------------------------------------------------------------------

uint8_t I2C_ReadBuffer(uint8_t addr, uint8_t *data, short dataSize) {

	uint8_t res;

	/* Send I2C address plus register address to the I2C bus *without* a stop condition */
	res = I2C2_MasterSendBlock(deviceData.handle, &addr, 1U, LDD_I2C_NO_SEND_STOP);

	if (res!=ERR_OK) {
		return ERR_FAILED;
	}

	while (!deviceData.dataTransmittedFlg) {} /* Wait until data is sent */

	deviceData.dataTransmittedFlg = FALSE;

	/* Receive InpData (1 byte) from the I2C bus and generates a stop condition to end transmission */
	res = I2C2_MasterReceiveBlock(deviceData.handle, data, dataSize, LDD_I2C_SEND_STOP);

	if (res!=ERR_OK) {
		return ERR_FAILED;
	}
	while (!deviceData.dataReceivedFlg) {} /* Wait until data is received received */

	deviceData.dataReceivedFlg = FALSE;

	return ERR_OK;
}
//--------------------------------------------------------------------------------------------

uint8_t I2C_Write(uint8_t addr, uint8_t val) {

	uint8_t buf[2], res;

	buf[0] = addr;
	buf[1] = val;

	res = I2C2_MasterSendBlock(deviceData.handle, &buf, 2U, LDD_I2C_SEND_STOP); /* Send OutData (3 bytes with address) on the I2C bus and generates not a stop condition to end transmission */

	if (res!=ERR_OK) {
		return ERR_FAILED;
	}

	while (!deviceData.dataTransmittedFlg) {}  /* Wait until date is sent */

	deviceData.dataTransmittedFlg = FALSE;

	return ERR_OK;
}
//--------------------------------------------------------------------------------------------

bool MMA845x_getXYZ(Accelerometer* acc){

	int8_t xyz[LEN_XYZ];

	uint8_t res = ERR_FAILED;
	uint8_t status;

	res = I2C_Read(MMA8451_STATUS,&status);

	if ((res==ERR_OK) && (status & MMA8451_ZYXDR_BIT)){

		res = I2C_ReadBuffer(MMA8451_OUT_X_MSB, (uint8_t*)&xyz,LEN_XYZ);

		if(res==ERR_OK){

			mma8451_range_t range = MMA845x_getRange();

			acc->x = toDecimal(xyz);
			acc->y = toDecimal(xyz+2);
			acc->z = toDecimal(xyz+4);

			float divider = 1.0;
			if (range == MMA8451_RANGE_8_G) divider = 1024.0;
			if (range == MMA8451_RANGE_4_G) divider = 2048.0;
			if (range == MMA8451_RANGE_2_G) divider = 4096.0;

			acc->x_g = (float)acc->x / divider;
			acc->y_g = (float)acc->y / divider;
			acc->z_g = (float)acc->z / divider;
		}
	}

	return res == ERR_OK;
}
//--------------------------------------------------------------------------------------------

void MMA845x_Standby (void)
{
	uint8_t ctrl;

	I2C_Read(MMA8451_CTRL_REG1, &ctrl);
	I2C_Write(MMA8451_CTRL_REG1, ctrl & (~MMA8451_ACTIVE_BIT));
}
//--------------------------------------------------------------------------------------------

void MMA845x_Active(void)
{
	uint8_t ctrl;

	I2C_Read(MMA8451_CTRL_REG1, &ctrl);
	I2C_Write(MMA8451_CTRL_REG1, ctrl | MMA8451_ACTIVE_BIT);
}
//--------------------------------------------------------------------------------------------

void MMA845x_init(void){

	uint8_t ctrlReg1,ctrlReg2;

	deviceData.handle = I2C2_Init(&deviceData);

	MMA845x_Standby();

	// Espelha registradores
	I2C_Read(MMA8451_CTRL_REG1,&ctrlReg1);
	I2C_Read(MMA8451_CTRL_REG2,&ctrlReg2);

	// High Resolution
	ctrlReg2 &= ~MMA8451_MODS_MASK;
	ctrlReg2 |= MMA8451_MODS1_BIT;

	// Hi e Lo indexados
	ctrlReg1 &= (~MMA8451_F_READ_BIT);

	//Low noise
	ctrlReg1 |= MMA8451_L_NOISE_BIT;

	// Date rate = 50Hz (20 ms)
	ctrlReg1 &= ~(MMA8451_DATARATE_MASK << 3);
	ctrlReg1 |= (MMA8451_DATARATE_50_HZ << 3);

	// Range
	I2C_Write(MMA8451_XYZ_DATA_CFG,  MMA8451_RANGE_2_G);

	// Atualiza registradores
	I2C_Write(MMA8451_CTRL_REG2,ctrlReg2);
	I2C_Write(MMA8451_CTRL_REG1,ctrlReg1);

	MMA845x_Active();
}
//--------------------------------------------------------------------------------------------

void MMA845x_deInit(void){

	I2C2_Deinit(deviceData.handle);
}
//--------------------------------------------------------------------------------------------

mma8451_range_t MMA845x_getRange(void){

	uint8_t data_cfg;

	if(I2C_Read(MMA8451_XYZ_DATA_CFG, &data_cfg)!=ERR_OK){

		return 0;
	}

	return (mma8451_range_t)(data_cfg  & 0x02);
}
//--------------------------------------------------------------------------------------------

uint8_t MMA845x_Reset(void)
{
	uint8_t res;
	uint8_t ctrlReg2;

	res = I2C_Write(MMA8451_CTRL_REG2, 0x40 );

	if(res ==ERR_OK){

		do{
			I2C_Read(MMA8451_CTRL_REG2,&ctrlReg2);
		}while (ctrlReg2 & 0x40);
	}

	return res;
}
//--------------------------------------------------------------------------------------------

uint8_t MMA845x_getOrientation( void )
{
    uint8_t orientation = 0;

    I2C_Read(MMA8451_PL_STATUS, &orientation);

    return orientation;
}
//--------------------------------------------------------------------------------------------

void MMA845x_setDataRate(mma8451_dataRate_t dataRate)
{
	uint8_t res;
	uint8_t ctrlReg1;

	res = I2C_Read(MMA8451_CTRL_REG1,&ctrlReg1);

	if(res==ERR_OK){

		MMA845x_Standby();

		ctrlReg1 &= ~(MMA8451_DATARATE_MASK << 3);
		ctrlReg1 |= (dataRate << 3);

		res = I2C_Write(MMA8451_CTRL_REG1,ctrlReg1);

		MMA845x_Active();

	}
}
//--------------------------------------------------------------------------------------------

void MMA845x_setNoise(bool flag)
{
	uint8_t res;
	uint8_t ctrlReg1;

	res = I2C_Read(MMA8451_CTRL_REG1,&ctrlReg1);

	if(res==ERR_OK){

		MMA845x_Standby();

		ctrlReg1 &= ~MMA8451_L_NOISE_BIT;

		if(flag){

			ctrlReg1 |= MMA8451_L_NOISE_BIT;
		}

		res = I2C_Write(MMA8451_CTRL_REG1,ctrlReg1);

		MMA845x_Active();
	}
}
//--------------------------------------------------------------------------------------------

void MMA845x_setRange(mma8451_range_t range){

  MMA845x_Standby();
  I2C_Write(MMA8451_XYZ_DATA_CFG, range & 0x2);
  MMA845x_Active();
}
//--------------------------------------------------------------------------------------------

short toDecimal (int8_t* hi_lo){

	short word = ((hi_lo[0]<<8) | hi_lo[1])>>2;

	if (hi_lo[0] > 0x7F){

		word = ((~word) + 1);
		word *= -1;
	}

	return word;
}
//--------------------------------------------------------------------------------------------