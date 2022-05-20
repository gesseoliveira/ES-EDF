#include "stm32f4xx.h"
#include "SPI.h"

/**
 * @brief This function handles SPI1 global interrupt.
 */
void SPI1_IRQHandler(void)
{
	uint8_t RX_Data;

	if((SPI1_Handler.Reception_Finished == 1) && (SPI1_Handler.Tranmission_Finished == 1))
	{
		SPI1_Handler.State = SPI_STATE_END_TX_RX;
		CLEAR_BIT(SPI1->CR2, SPI_CR2_TXEIE);	// TX empty interrupt [SPI ready to transmit data]
		SPI1_Handler.Reception_Finished = 0;
		SPI1_Handler.Tranmission_Finished = 0;
	}

	// SPI1 Data Reception control
	if((SPI1->SR & SPI_SR_RXNE) && (SPI1->CR2 & SPI_CR2_RXNEIE))
	{
		RX_Data = SPI1->DR;
		if(SPI1_Handler.RX_Size > 0)
		{
			if(SPI1_Handler.Rx_Buffer != NULL)
			{
				*SPI1_Handler.Rx_Buffer = RX_Data;
				SPI1_Handler.Rx_Buffer++;
			}
			SPI1_Handler.RX_Size --;
		}

		if(SPI1_Handler.RX_Size == 0)
		{
			SPI1_Handler.Reception_Finished = 1;
			CLEAR_BIT(SPI1->CR2, SPI_CR2_RXNEIE);
		}
		else
		{
			if((SPI1_Handler.TX_Size == 0) && (SPI1->SR & SPI_SR_TXE))
				SPI1->DR = SPI1_Handler.Dummy_Byte;
		}
	}

	// SPI1 Data transmission control
	if((SPI1->SR & SPI_SR_TXE) && (SPI1->CR2 & SPI_CR2_TXEIE))
	{
		if(SPI1_Handler.TX_Size == 0)
		{
			SPI1_Handler.Tranmission_Finished = 1;
		}
		if(SPI1_Handler.TX_Size > 0)
		{
			if(SPI1_Handler.Tx_Buffer != NULL)
				SPI1->DR = *SPI1_Handler.Tx_Buffer++;
			else
				SPI1->DR = SPI1_Handler.Dummy_Byte;
			SPI1_Handler.TX_Size --;
		}
	}
}

/**
 * @brief This function handles SPI2 global interrupt.
 */
void SPI2_IRQHandler(void)
{
	uint8_t RX_Data;

	if((SPI2_Handler.Reception_Finished == 1) && (SPI2_Handler.Tranmission_Finished == 1))
	{
		SPI2_Handler.State = SPI_STATE_END_TX_RX;
		CLEAR_BIT(SPI2->CR2, SPI_CR2_TXEIE);	// TX empty interrupt [SPI ready to transmit data]
		SPI2_Handler.Reception_Finished = 0;
		SPI2_Handler.Tranmission_Finished = 0;
	}

	// SPI1 Data Reception control
	if((SPI2->SR & SPI_SR_RXNE) && (SPI2->CR2 & SPI_CR2_RXNEIE))
	{
		RX_Data = SPI2->DR;
		if(SPI2_Handler.RX_Size > 0)
		{
			if(SPI2_Handler.Rx_Buffer != NULL)
			{
				*SPI2_Handler.Rx_Buffer = RX_Data;
				SPI2_Handler.Rx_Buffer++;
			}
			SPI2_Handler.RX_Size --;
		}

		if(SPI2_Handler.RX_Size == 0)
		{
			SPI2_Handler.Reception_Finished = 1;
			CLEAR_BIT(SPI2->CR2, SPI_CR2_RXNEIE);
		}
		else
		{
			if((SPI2_Handler.TX_Size == 0) && (SPI2->SR & SPI_SR_TXE))
				SPI2->DR = SPI2_Handler.Dummy_Byte;
		}
	}

	// SPI2 Data transmission control
	if((SPI2->SR & SPI_SR_TXE) && (SPI2->CR2 & SPI_CR2_TXEIE))
	{
		if(SPI2_Handler.TX_Size == 0)
		{
			SPI2_Handler.Tranmission_Finished = 1;
		}
		if(SPI2_Handler.TX_Size > 0)
		{
			if(SPI2_Handler.Tx_Buffer != NULL)
				SPI2->DR = *SPI2_Handler.Tx_Buffer++;
			else
				SPI2->DR = SPI2_Handler.Dummy_Byte;
			SPI2_Handler.TX_Size --;
		}
	}
}

/**
 * @brief This function handles SPI3 global interrupt.
 */
void SPI3_IRQHandler(void)
{
	uint8_t RX_Data;

	if((SPI3_Handler.Reception_Finished == 1) && (SPI3_Handler.Tranmission_Finished == 1))
	{
		SPI3_Handler.State = SPI_STATE_END_TX_RX;
		CLEAR_BIT(SPI3->CR2, SPI_CR2_TXEIE);	// TX empty interrupt [SPI ready to transmit data]
		SPI3_Handler.Reception_Finished = 0;
		SPI3_Handler.Tranmission_Finished = 0;
	}

	// SPI3 Data Reception control
	if((SPI3->SR & SPI_SR_RXNE) && (SPI3->CR2 & SPI_CR2_RXNEIE))
	{
		RX_Data = SPI3->DR;
		if(SPI3_Handler.RX_Size > 0)
		{
			if(SPI3_Handler.Rx_Buffer != NULL)
			{
				*SPI3_Handler.Rx_Buffer = RX_Data;
				SPI3_Handler.Rx_Buffer++;
			}
			SPI3_Handler.RX_Size --;
		}

		if(SPI3_Handler.RX_Size == 0)
		{
			SPI3_Handler.Reception_Finished = 1;
			CLEAR_BIT(SPI3->CR2, SPI_CR2_RXNEIE);
		}
		else
		{
			if((SPI3_Handler.TX_Size == 0) && (SPI3->SR & SPI_SR_TXE))
				SPI3->DR = SPI3_Handler.Dummy_Byte;
		}
	}

	// SPI3 Data transmission control
	if((SPI3->SR & SPI_SR_TXE) && (SPI3->CR2 & SPI_CR2_TXEIE))
	{
		if(SPI3_Handler.TX_Size == 0)
		{
			SPI3_Handler.Tranmission_Finished = 1;
		}
		if(SPI3_Handler.TX_Size > 0)
		{
			if(SPI3_Handler.Tx_Buffer != NULL)
				SPI3->DR = *SPI3_Handler.Tx_Buffer++;
			else
				SPI3->DR = SPI3_Handler.Dummy_Byte;
			SPI3_Handler.TX_Size --;
		}
	}
}
