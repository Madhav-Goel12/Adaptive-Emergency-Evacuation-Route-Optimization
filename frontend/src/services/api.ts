import type { Alert, Road, RouteResult, Weather } from '../types'

export interface RouteRequest {
  startId: string
  destinationId: string
  startCoordinates?: [number, number]
  destinationCoordinates?: [number, number]
}

async function request<T>(path: string, options?: RequestInit): Promise<T> {
  const headers = new Headers(options?.headers)
  headers.set('Accept', 'application/json')
  if (options?.body != null && !headers.has('Content-Type')) {
    headers.set('Content-Type', 'application/json')
  }

  let response: Response
  try {
    // Relative URLs intentionally use the Vite /api proxy in Codespaces.
    response = await fetch(path, { ...options, headers })
  } catch (error) {
    console.error(`Network error for ${path}:`, error)
    throw new Error(`Cannot reach ${path}. Confirm the C++ backend is running on port 8080.`)
  }

  const text = await response.text()
  if (!response.ok) {
    let message = text
    try {
      const parsed = JSON.parse(text)
      message = parsed.error ?? parsed.message ?? text
    } catch { /* use response text */ }
    throw new Error(`HTTP ${response.status}: ${message || 'Request failed'}`)
  }
  try {
    return JSON.parse(text) as T
  } catch {
    throw new Error(`Invalid JSON returned by ${path}`)
  }
}

export const api = {
  getRoads: () => request<Road[]>('/api/roads'),
  getWeather: () => request<Weather>('/api/weather'),
  getAlerts: () => request<Alert[]>('/api/alerts'),
  getRoute: (data: RouteRequest) => request<RouteResult>('/api/route', {
    method: 'POST',
    body: JSON.stringify(data),
  }),
}
