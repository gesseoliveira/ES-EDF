/**
 ******************************************************************************
 * @file    IIC.h
 * @author  Plinio Barbosa da Silva
 * @brief   IIC module driver.
 * @brief   Version 2019.11.2
 * @brief   Creation Date 16/07/2019
 *
 *          This file provides functions to manage IIC Interrupts
 */
#include "IIC.h"
#include "stm32f4xx.h"
#include "stddef.h"

void I2C1_EV_IRQHandler(void)
{
	uint32_t sr1itflags				= 0U;
	uint32_t sr2itflags             = 0U;
	uint32_t itsources              = 0U;

	itsources              	= READ_REG(I2C1->CR2);
	sr2itflags   			= READ_REG(I2C1->SR2);
	sr1itflags   			= READ_REG(I2C1->SR1);

	/* SB Set ----------------------------------------------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_SB) != RESET) && (READ_BIT(itsources, I2C_CR2_ITEVTEN) != RESET))
	{
		if( I2C1_Handler.Mode	== IIC_WRITE )
			I2C1->DR = (I2C1_Handler.Dev_Address & 0xFE);
		else if( I2C1_Handler.Mode	== IIC_READ )
			I2C1->DR = (I2C1_Handler.Dev_Address | 0x01);
	}

	/* ADDR Set --------------------------------------------------------------*/
	else if ((READ_BIT(sr1itflags, I2C_SR1_ADDR) != RESET) && (READ_BIT(itsources, I2C_CR2_ITEVTEN) != RESET))
	{
		sr1itflags   			= READ_REG(I2C1->SR1);
		sr2itflags   			= READ_REG(I2C1->SR2);
	}
	/* I2C in mode Transmitter -----------------------------------------------*/
	else if (READ_BIT(sr2itflags, I2C_SR2_TRA) != RESET)
	{
		/* TXE set and BTF reset -----------------------------------------------*/
		if ((READ_BIT(sr1itflags, I2C_SR1_TXE) != RESET) && (READ_BIT(itsources, I2C_CR2_ITBUFEN) != RESET) && (READ_BIT(sr1itflags, I2C_SR1_BTF) == RESET))
		{
			if(I2C1_Handler.Register_Address_Size != 0)
			{
				I2C1->DR	=	I2C1_Handler.Register_Address & 0xFF;
				I2C1_Handler.Register_Address >>= 8;
				I2C1_Handler.Register_Address_Size --;
			}
			else
			{
				if(I2C1_Handler.TxSize != 0)
				{
					I2C1->DR = *I2C1_Handler.pTxBuffer;
					I2C1_Handler.pTxBuffer  ++;
					I2C1_Handler.TxSize --;
				}
			}
		}
		/* BTF set -------------------------------------------------------------*/
		else if ((READ_BIT(sr1itflags, I2C_SR1_BTF) != RESET) && (READ_BIT(itsources, I2C_CR2_ITEVTEN) != RESET))
		{
			if((I2C1_Handler.Op_Type == IIC_MEM_WRITE) && (I2C1_Handler.TxSize == 0))
			{
				I2C1_Handler.Status = IIC_IDLE;
				SET_BIT(I2C1->CR1, I2C_CR1_STOP);
				CLEAR_BIT(I2C1->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
				if(I2C1_Handler.IIC_Operation_Write_Finished_Callback != NULL) // If callback is enabled
				{
					I2C1_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
					I2C1_Handler.IIC_Operation_Write_Finished_Callback();
				}
			}
			else if ((I2C1_Handler.Op_Type == IIC_MEM_READ) && (I2C1_Handler.TxSize == 0))
			{
				I2C1_Handler.Mode	= IIC_READ;
				if(I2C1_Handler.RxSize > 1)
					SET_BIT(I2C1->CR1, I2C_CR1_ACK);
				else
					CLEAR_BIT(I2C1->CR1, I2C_CR1_ACK);
				SET_BIT(I2C1->CR1, I2C_CR1_START);
			}
		}
	}
	else
	{
		/* RXNE set and BTF reset -----------------------------------------------*/
		if ((READ_BIT(sr1itflags, I2C_SR1_RXNE) != RESET) && (READ_BIT(itsources, I2C_CR2_ITBUFEN) != RESET) && (READ_BIT(sr1itflags, I2C_SR1_BTF) == RESET))
		{
			if(I2C1_Handler.RxSize != 0)
			{
				*I2C1_Handler.pRxBuffer = I2C1->DR;
				I2C1_Handler.pRxBuffer ++;
				I2C1_Handler.RxSize --;
			}
			if (I2C1_Handler.RxSize == 0)
			{
				CLEAR_BIT(I2C1->CR1, I2C_CR1_ACK);
				SET_BIT(I2C1->CR1, I2C_CR1_STOP);
				I2C1_Handler.Status = IIC_IDLE;
				CLEAR_BIT(I2C1->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
				if(I2C1_Handler.IIC_Operation_Read_Finished_Callback != NULL)
				{
					I2C1_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
					I2C1_Handler.IIC_Operation_Read_Finished_Callback();
				}
			}
		}
		/* BTF set -------------------------------------------------------------*/
		else if ((READ_BIT(sr1itflags, I2C_SR1_BTF) != RESET) && (READ_BIT(itsources, I2C_CR2_ITEVTEN) != RESET))
		{

		}
	}
}

void I2C1_ER_IRQHandler(void)
{
	uint32_t sr1itflags = READ_REG(I2C1->SR1);
	uint32_t itsources  = READ_REG(I2C1->CR2);

	/* I2C Bus error interrupt occurred ----------------------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_BERR) != RESET) && (READ_BIT(itsources, I2C_CR2_ITERREN) != RESET))
	{
		SET_BIT(I2C1->CR1, I2C_CR1_STOP);
		CLEAR_BIT(I2C1->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
		if(I2C1_Handler.IIC_Operation_Error_Callback != NULL)
		{
			I2C1_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
			I2C1_Handler.IIC_Operation_Error_Callback();
		}
		else
		{
			I2C1_Handler.Status = IIC_ERROR;
		}
		/* Clear BERR flag */
		CLEAR_BIT(I2C1->SR1, I2C_SR1_BERR);
	}

	/* I2C Arbitration Lost error interrupt occurred ---------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_ARLO) != RESET) && (READ_BIT(itsources, I2C_CR2_ITERREN) != RESET))
	{
		SET_BIT(I2C1->CR1, I2C_CR1_STOP);
		CLEAR_BIT(I2C1->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
		if(I2C1_Handler.IIC_Operation_Error_Callback != NULL)
		{
			I2C1_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
			I2C1_Handler.IIC_Operation_Error_Callback();
		}
		else
		{
			I2C1_Handler.Status = IIC_ERROR;
		}
		/* Clear ARLO flag */
		CLEAR_BIT(I2C1->SR1, I2C_SR1_ARLO);
	}

	/* I2C Acknowledge failure error interrupt occurred ------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_AF) != RESET) && (READ_BIT(itsources, I2C_CR2_ITERREN) != RESET))
	{
		SET_BIT(I2C1->CR1, I2C_CR1_STOP);
		CLEAR_BIT(I2C1->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
		if(I2C1_Handler.IIC_Operation_Error_Callback != NULL)
		{
			I2C1_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
			I2C1_Handler.IIC_Operation_Error_Callback();
		}
		else
		{
			I2C1_Handler.Status = IIC_ERROR;
		}
		/* Clear AF flag */
		CLEAR_BIT(I2C1->SR1, I2C_SR1_AF);
	}

	/* I2C Over-Run/Under-Run interrupt occurred -------------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_OVR) != RESET) && (READ_BIT(itsources, I2C_CR2_ITERREN) != RESET))
	{
		SET_BIT(I2C1->CR1, I2C_CR1_STOP);
		CLEAR_BIT(I2C1->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
		if(I2C1_Handler.IIC_Operation_Error_Callback != NULL)
		{
			I2C1_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
			I2C1_Handler.IIC_Operation_Error_Callback();
		}
		else
		{
			I2C1_Handler.Status = IIC_ERROR;
		}
		CLEAR_BIT(I2C1->SR1, I2C_SR1_OVR);
	}
}

void I2C2_EV_IRQHandler(void)
{
	uint32_t sr1itflags				= 0U;
	uint32_t sr2itflags             = 0U;
	uint32_t itsources              = 0U;

	itsources              	= READ_REG(I2C2->CR2);
	sr2itflags   			= READ_REG(I2C2->SR2);
	sr1itflags   			= READ_REG(I2C2->SR1);

	/* SB Set ----------------------------------------------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_SB) != RESET) && (READ_BIT(itsources, I2C_CR2_ITEVTEN) != RESET))
	{
		if( I2C2_Handler.Mode	== IIC_WRITE )
			I2C2->DR = (I2C2_Handler.Dev_Address & 0xFE);
		else if( I2C2_Handler.Mode	== IIC_READ )
			I2C2->DR = (I2C2_Handler.Dev_Address | 0x01);
	}

	/* ADDR Set --------------------------------------------------------------*/
	else if ((READ_BIT(sr1itflags, I2C_SR1_ADDR) != RESET) && (READ_BIT(itsources, I2C_CR2_ITEVTEN) != RESET))
	{
		sr1itflags   			= READ_REG(I2C2->SR1);
		sr2itflags   			= READ_REG(I2C2->SR2);
	}
	/* I2C in mode Transmitter -----------------------------------------------*/
	else if (READ_BIT(sr2itflags, I2C_SR2_TRA) != RESET)
	{
		/* TXE set and BTF reset -----------------------------------------------*/
		if ((READ_BIT(sr1itflags, I2C_SR1_TXE) != RESET) && (READ_BIT(itsources, I2C_CR2_ITBUFEN) != RESET) && (READ_BIT(sr1itflags, I2C_SR1_BTF) == RESET))
		{
			if(I2C2_Handler.Register_Address_Size != 0)
			{
				I2C2->DR	=	I2C2_Handler.Register_Address & 0xFF;
				I2C2_Handler.Register_Address >>= 8;
				I2C2_Handler.Register_Address_Size --;
			}
			else
			{
				if(I2C2_Handler.TxSize != 0)
				{
					I2C2->DR = *I2C2_Handler.pTxBuffer;
					I2C2_Handler.pTxBuffer  ++;
					I2C2_Handler.TxSize --;
				}
			}
		}
		/* BTF set -------------------------------------------------------------*/
		else if ((READ_BIT(sr1itflags, I2C_SR1_BTF) != RESET) && (READ_BIT(itsources, I2C_CR2_ITEVTEN) != RESET))
		{
			if((I2C2_Handler.Op_Type == IIC_MEM_WRITE) && (I2C2_Handler.TxSize == 0))
			{
				I2C2_Handler.Status = IIC_IDLE;
				SET_BIT(I2C2->CR1, I2C_CR1_STOP);
				CLEAR_BIT(I2C2->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
				if(I2C2_Handler.IIC_Operation_Write_Finished_Callback != NULL) // If callback is enabled
				{
					I2C2_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
					I2C2_Handler.IIC_Operation_Write_Finished_Callback();
				}
			}
			else if ((I2C2_Handler.Op_Type == IIC_MEM_READ) && (I2C2_Handler.TxSize == 0))
			{
				I2C2_Handler.Mode	= IIC_READ;
				if(I2C2_Handler.RxSize > 1)
					SET_BIT(I2C2->CR1, I2C_CR1_ACK);
				else
					CLEAR_BIT(I2C2->CR1, I2C_CR1_ACK);
				SET_BIT(I2C2->CR1, I2C_CR1_START);
			}
		}
	}
	else
	{
		/* RXNE set and BTF reset -----------------------------------------------*/
		if ((READ_BIT(sr1itflags, I2C_SR1_RXNE) != RESET) && (READ_BIT(itsources, I2C_CR2_ITBUFEN) != RESET) && (READ_BIT(sr1itflags, I2C_SR1_BTF) == RESET))
		{
			if(I2C2_Handler.RxSize != 0)
			{
				*I2C2_Handler.pRxBuffer = I2C2->DR;
				I2C2_Handler.pRxBuffer ++;
				I2C2_Handler.RxSize --;
			}
			if (I2C2_Handler.RxSize == 0)
			{
				CLEAR_BIT(I2C2->CR1, I2C_CR1_ACK);
				SET_BIT(I2C2->CR1, I2C_CR1_STOP);
				I2C2_Handler.Status = IIC_IDLE;
				CLEAR_BIT(I2C2->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
				if(I2C2_Handler.IIC_Operation_Read_Finished_Callback != NULL)
				{
					I2C2_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
					I2C2_Handler.IIC_Operation_Read_Finished_Callback();
				}
			}
		}
		/* BTF set -------------------------------------------------------------*/
		else if ((READ_BIT(sr1itflags, I2C_SR1_BTF) != RESET) && (READ_BIT(itsources, I2C_CR2_ITEVTEN) != RESET))
		{

		}
	}
}

void I2C2_ER_IRQHandler(void)
{
	uint32_t sr1itflags = READ_REG(I2C2->SR1);
	uint32_t itsources  = READ_REG(I2C2->CR2);

	/* I2C Bus error interrupt occurred ----------------------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_BERR) != RESET) && (READ_BIT(itsources, I2C_CR2_ITERREN) != RESET))
	{
		SET_BIT(I2C2->CR1, I2C_CR1_STOP);
		CLEAR_BIT(I2C2->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
		if(I2C2_Handler.IIC_Operation_Error_Callback != NULL)
		{
			I2C2_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
			I2C2_Handler.IIC_Operation_Error_Callback();
		}
		else
		{
			I2C2_Handler.Status = IIC_ERROR;
		}
		/* Clear BERR flag */
		CLEAR_BIT(I2C2->SR1, I2C_SR1_BERR);
	}

	/* I2C Arbitration Lost error interrupt occurred ---------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_ARLO) != RESET) && (READ_BIT(itsources, I2C_CR2_ITERREN) != RESET))
	{
		SET_BIT(I2C2->CR1, I2C_CR1_STOP);
		CLEAR_BIT(I2C2->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
		if(I2C2_Handler.IIC_Operation_Error_Callback != NULL)
		{
			I2C2_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
			I2C2_Handler.IIC_Operation_Error_Callback();
		}
		else
		{
			I2C2_Handler.Status = IIC_ERROR;
		}
		/* Clear ARLO flag */
		CLEAR_BIT(I2C2->SR1, I2C_SR1_ARLO);
	}

	/* I2C Acknowledge failure error interrupt occurred ------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_AF) != RESET) && (READ_BIT(itsources, I2C_CR2_ITERREN) != RESET))
	{
		SET_BIT(I2C2->CR1, I2C_CR1_STOP);
		CLEAR_BIT(I2C2->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
		if(I2C2_Handler.IIC_Operation_Error_Callback != NULL)
		{
			I2C2_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
			I2C2_Handler.IIC_Operation_Error_Callback();
		}
		else
		{
			I2C2_Handler.Status = IIC_ERROR;
		}
		/* Clear AF flag */
		CLEAR_BIT(I2C2->SR1, I2C_SR1_AF);
	}

	/* I2C Over-Run/Under-Run interrupt occurred -------------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_OVR) != RESET) && (READ_BIT(itsources, I2C_CR2_ITERREN) != RESET))
	{
		SET_BIT(I2C2->CR1, I2C_CR1_STOP);
		CLEAR_BIT(I2C2->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
		if(I2C2_Handler.IIC_Operation_Error_Callback != NULL)
		{
			I2C2_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
			I2C2_Handler.IIC_Operation_Error_Callback();
		}
		else
		{
			I2C2_Handler.Status = IIC_ERROR;
		}
		CLEAR_BIT(I2C2->SR1, I2C_SR1_OVR);
	}
}

void I2C3_EV_IRQHandler(void)
{
	uint32_t sr1itflags				= 0U;
	uint32_t sr2itflags             = 0U;
	uint32_t itsources              = 0U;

	itsources              	= READ_REG(I2C3->CR2);
	sr2itflags   			= READ_REG(I2C3->SR2);
	sr1itflags   			= READ_REG(I2C3->SR1);

	/* SB Set ----------------------------------------------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_SB) != RESET) && (READ_BIT(itsources, I2C_CR2_ITEVTEN) != RESET))
	{
		if( I2C3_Handler.Mode	== IIC_WRITE )
			I2C3->DR = (I2C3_Handler.Dev_Address & 0xFE);
		else if( I2C3_Handler.Mode	== IIC_READ )
			I2C3->DR = (I2C3_Handler.Dev_Address | 0x01);
	}

	/* ADDR Set --------------------------------------------------------------*/
	else if ((READ_BIT(sr1itflags, I2C_SR1_ADDR) != RESET) && (READ_BIT(itsources, I2C_CR2_ITEVTEN) != RESET))
	{
		sr1itflags   			= READ_REG(I2C3->SR1);
		sr2itflags   			= READ_REG(I2C3->SR2);
	}
	/* I2C in mode Transmitter -----------------------------------------------*/
	else if (READ_BIT(sr2itflags, I2C_SR2_TRA) != RESET)
	{
		/* TXE set and BTF reset -----------------------------------------------*/
		if ((READ_BIT(sr1itflags, I2C_SR1_TXE) != RESET) && (READ_BIT(itsources, I2C_CR2_ITBUFEN) != RESET) && (READ_BIT(sr1itflags, I2C_SR1_BTF) == RESET))
		{
			if(I2C3_Handler.Register_Address_Size != 0)
			{
				I2C3->DR	=	I2C3_Handler.Register_Address & 0xFF;
				I2C3_Handler.Register_Address >>= 8;
				I2C3_Handler.Register_Address_Size --;
			}
			else
			{
				if(I2C3_Handler.TxSize != 0)
				{
					I2C3->DR = *I2C3_Handler.pTxBuffer;
					I2C3_Handler.pTxBuffer  ++;
					I2C3_Handler.TxSize --;
				}
			}
		}
		/* BTF set -------------------------------------------------------------*/
		else if ((READ_BIT(sr1itflags, I2C_SR1_BTF) != RESET) && (READ_BIT(itsources, I2C_CR2_ITEVTEN) != RESET))
		{
			if((I2C3_Handler.Op_Type == IIC_MEM_WRITE) && (I2C3_Handler.TxSize == 0))
			{
				I2C3_Handler.Status = IIC_IDLE;
				SET_BIT(I2C3->CR1, I2C_CR1_STOP);
				CLEAR_BIT(I2C3->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
				if(I2C3_Handler.IIC_Operation_Write_Finished_Callback != NULL) // If callback is enabled
				{
					I2C3_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
					I2C3_Handler.IIC_Operation_Write_Finished_Callback();
				}
			}
			else if ((I2C3_Handler.Op_Type == IIC_MEM_READ) && (I2C3_Handler.TxSize == 0))
			{
				I2C3_Handler.Mode	= IIC_READ;
				if(I2C3_Handler.RxSize > 1)
					SET_BIT(I2C3->CR1, I2C_CR1_ACK);
				else
					CLEAR_BIT(I2C3->CR1, I2C_CR1_ACK);
				SET_BIT(I2C3->CR1, I2C_CR1_START);
			}
		}
	}
	else
	{
		/* RXNE set and BTF reset -----------------------------------------------*/
		if ((READ_BIT(sr1itflags, I2C_SR1_RXNE) != RESET) && (READ_BIT(itsources, I2C_CR2_ITBUFEN) != RESET) && (READ_BIT(sr1itflags, I2C_SR1_BTF) == RESET))
		{
			if(I2C3_Handler.RxSize != 0)
			{
				*I2C3_Handler.pRxBuffer = I2C3->DR;
				I2C3_Handler.pRxBuffer ++;
				I2C3_Handler.RxSize --;
			}
			if (I2C3_Handler.RxSize == 0)
			{
				CLEAR_BIT(I2C3->CR1, I2C_CR1_ACK);
				SET_BIT(I2C3->CR1, I2C_CR1_STOP);
				I2C3_Handler.Status = IIC_IDLE;
				CLEAR_BIT(I2C3->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
				if(I2C3_Handler.IIC_Operation_Read_Finished_Callback != NULL)
				{
					I2C3_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
					I2C3_Handler.IIC_Operation_Read_Finished_Callback();
				}
			}
		}
		/* BTF set -------------------------------------------------------------*/
		else if ((READ_BIT(sr1itflags, I2C_SR1_BTF) != RESET) && (READ_BIT(itsources, I2C_CR2_ITEVTEN) != RESET))
		{

		}
	}
}

void I2C3_ER_IRQHandler(void)
{
	uint32_t sr1itflags = READ_REG(I2C3->SR1);
	uint32_t itsources  = READ_REG(I2C3->CR2);

	/* I2C Bus error interrupt occurred ----------------------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_BERR) != RESET) && (READ_BIT(itsources, I2C_CR2_ITERREN) != RESET))
	{
		SET_BIT(I2C3->CR1, I2C_CR1_STOP);
		CLEAR_BIT(I2C3->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
		if(I2C3_Handler.IIC_Operation_Error_Callback != NULL)
		{
			I2C3_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
			I2C3_Handler.IIC_Operation_Error_Callback();
		}
		else
		{
			I2C3_Handler.Status = IIC_ERROR;
		}
		/* Clear BERR flag */
		CLEAR_BIT(I2C3->SR1, I2C_SR1_BERR);
	}

	/* I2C Arbitration Lost error interrupt occurred ---------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_ARLO) != RESET) && (READ_BIT(itsources, I2C_CR2_ITERREN) != RESET))
	{
		SET_BIT(I2C3->CR1, I2C_CR1_STOP);
		CLEAR_BIT(I2C3->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
		if(I2C3_Handler.IIC_Operation_Error_Callback != NULL)
		{
			I2C3_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
			I2C3_Handler.IIC_Operation_Error_Callback();
		}
		else
		{
			I2C3_Handler.Status = IIC_ERROR;
		}
		/* Clear ARLO flag */
		CLEAR_BIT(I2C3->SR1, I2C_SR1_ARLO);
	}

	/* I2C Acknowledge failure error interrupt occurred ------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_AF) != RESET) && (READ_BIT(itsources, I2C_CR2_ITERREN) != RESET))
	{
		SET_BIT(I2C3->CR1, I2C_CR1_STOP);
		CLEAR_BIT(I2C3->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
		if(I2C3_Handler.IIC_Operation_Error_Callback != NULL)
		{
			I2C3_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
			I2C3_Handler.IIC_Operation_Error_Callback();
		}
		else
		{
			I2C3_Handler.Status = IIC_ERROR;
		}
		/* Clear AF flag */
		CLEAR_BIT(I2C3->SR1, I2C_SR1_AF);
	}

	/* I2C Over-Run/Under-Run interrupt occurred -------------------------------*/
	if ((READ_BIT(sr1itflags, I2C_SR1_OVR) != RESET) && (READ_BIT(itsources, I2C_CR2_ITERREN) != RESET))
	{
		SET_BIT(I2C3->CR1, I2C_CR1_STOP);
		CLEAR_BIT(I2C3->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
		if(I2C3_Handler.IIC_Operation_Error_Callback != NULL)
		{
			I2C3_Handler.State_Machine_IIC = STATE_IIC_Prepare_Operation;
			I2C3_Handler.IIC_Operation_Error_Callback();
		}
		else
		{
			I2C3_Handler.Status = IIC_ERROR;
		}
		CLEAR_BIT(I2C3->SR1, I2C_SR1_OVR);
	}
}
